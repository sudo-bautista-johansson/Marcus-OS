#include <stdint.h>

#define NETWORK_MTU 1500
#define ETHERNET_HEADER 14
#define IPV4_HEADER 20
#define UDP_HEADER 8
#define ARP_PACKET 42
#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc
#define E1000_VENDOR 0x8086
#define E1000_VIRTUAL_BASE 0x2000000ULL
#define E1000_CTRL 0x0000
#define E1000_STATUS 0x0008
#define E1000_ICR 0x00c0
#define E1000_IMS 0x00d0
#define E1000_IMC 0x00d8
#define E1000_RCTL 0x0100
#define E1000_TCTL 0x0400
#define E1000_TIPG 0x0410
#define E1000_RDBAL 0x2800
#define E1000_RDH 0x2810
#define E1000_RDT 0x2818
#define E1000_RXDCTL 0x2828
#define E1000_TDBAL 0x3800
#define E1000_TDH 0x3810
#define E1000_TDT 0x3818
#define E1000_TXDCTL 0x3828
#define E1000_MTA 0x5200
#define E1000_RAL 0x5400
#define E1000_RAH 0x5404
#define E1000_CTRL_SLU 0x00000040u
#define E1000_CTRL_RST 0x04000000u
#define E1000_STATUS_LU 0x00000002u
#define E1000_RCTL_EN 0x00000002u
#define E1000_RCTL_BAM 0x00008000u
#define E1000_RCTL_SECRC 0x04000000u
#define E1000_TCTL_EN 0x00000002u
#define E1000_TCTL_PSP 0x00000008u
#define E1000_TX_CMD_EOP 0x01
#define E1000_TX_CMD_IFCS 0x02
#define E1000_TX_CMD_RS 0x08
#define E1000_DESC_DD 0x01
#define E1000_RX_EOP 0x02
#define E1000_RING_SIZE 8
#define E1000_RING_BYTES 128
#define GUEST_IP 0x0a00020fu
#define GATEWAY_IP 0x0a000202u

typedef struct {
    uint32_t ip;
    uint8_t mac[6];
    uint8_t valid;
} ArpEntry;

typedef struct {
    uint16_t local_port;
    uint16_t remote_port;
    uint32_t remote_ip;
    uint8_t active;
} UdpSocket;

typedef struct {
    uint64_t buffer_address;
    uint16_t length;
    uint8_t checksum_offset;
    uint8_t command;
    uint8_t status;
    uint8_t checksum_start;
    uint16_t special;
} __attribute__((packed)) E1000TxDesc;

typedef struct {
    uint64_t buffer_address;
    uint16_t length;
    uint16_t checksum;
    uint8_t status;
    uint8_t errors;
    uint16_t special;
} __attribute__((packed)) E1000RxDesc;

typedef struct {
    uint8_t bytes[NETWORK_MTU + ETHERNET_HEADER];
    uint16_t length;
} NetworkBuffer;

_Static_assert(sizeof(E1000TxDesc) == 16, "TX descriptor must be 16 bytes");
_Static_assert(sizeof(E1000RxDesc) == 16, "RX descriptor must be 16 bytes");

static ArpEntry arp_cache[8];
static UdpSocket sockets[8];
static uint64_t e1000_mmio_base;
static uint16_t e1000_device;
static uint8_t e1000_present;
static uintptr_t e1000_virtual_base;
static uint8_t e1000_init_stage;
static uint8_t e1000_ring_stage;
static uint8_t e1000_wire_stage;
static uint8_t e1000_pci_bus;
static uint8_t e1000_pci_slot;
static uint8_t local_mac[6];
static uint8_t tx_next;
static uint8_t rx_next;
static uint32_t e1000_last_tdh;
static uint32_t e1000_last_tdt;
static uint32_t e1000_last_txdctl;
static uint32_t e1000_last_tctl;
static uint32_t e1000_last_icr;
static volatile uint32_t e1000_irq_count;

static E1000RxDesc rx_ring[E1000_RING_SIZE] __attribute__((aligned(16), section(".dma")));
static E1000TxDesc tx_ring[E1000_RING_SIZE] __attribute__((aligned(16), section(".dma")));
static uint8_t rx_buffers[E1000_RING_SIZE][2048] __attribute__((aligned(16), section(".dma")));
static uint8_t tx_buffers[E1000_RING_SIZE][2048] __attribute__((aligned(16), section(".dma")));

extern int kernel_mmio_map(uintptr_t virtual_address, uintptr_t physical_address, uint16_t page_count);

static uint16_t checksum(const uint8_t *data, uint16_t length) {
    uint32_t sum = 0;
    for (uint16_t index = 0; index + 1 < length; index += 2) {
        sum += ((uint16_t)data[index] << 8) | data[index + 1];
    }
    if ((length & 1) != 0) sum += (uint16_t)data[length - 1] << 8;
    while ((sum >> 16) != 0) sum = (sum & 0xffff) + (sum >> 16);
    return (uint16_t)~sum;
}

static void write_u16(uint8_t *destination, uint16_t value) {
    destination[0] = (uint8_t)(value >> 8);
    destination[1] = (uint8_t)value;
}

static uint16_t read_u16(const uint8_t *source) {
    return (uint16_t)(((uint16_t)source[0] << 8) | source[1]);
}

static void write_u32(uint8_t *destination, uint32_t value) {
    destination[0] = (uint8_t)(value >> 24);
    destination[1] = (uint8_t)(value >> 16);
    destination[2] = (uint8_t)(value >> 8);
    destination[3] = (uint8_t)value;
}

static uint32_t read_u32(const uint8_t *source) {
    return ((uint32_t)source[0] << 24) | ((uint32_t)source[1] << 16) |
        ((uint32_t)source[2] << 8) | source[3];
}

static void copy_bytes(uint8_t *destination, const uint8_t *source, uint16_t length) {
    for (uint16_t index = 0; index < length; ++index) destination[index] = source[index];
}

static void ethernet_header(NetworkBuffer *packet, uint16_t ether_type) {
    for (uint8_t index = 0; index < 6; ++index) {
        packet->bytes[index] = 0xff;
        packet->bytes[6 + index] = 0x52;
    }
    write_u16(&packet->bytes[12], ether_type);
}

static int build_udp_packet(NetworkBuffer *packet, uint32_t source_ip, uint32_t destination_ip,
                            uint16_t source_port, uint16_t destination_port,
                            const uint8_t *payload, uint16_t payload_length) {
    if (payload_length > NETWORK_MTU - IPV4_HEADER - UDP_HEADER) return 0;
    ethernet_header(packet, 0x0800);
    uint8_t *ip = &packet->bytes[ETHERNET_HEADER];
    ip[0] = 0x45; ip[1] = 0;
    write_u16(&ip[2], IPV4_HEADER + UDP_HEADER + payload_length);
    write_u16(&ip[4], 0x1234); write_u16(&ip[6], 0x4000);
    ip[8] = 64; ip[9] = 17;
    write_u16(&ip[10], 0);
    write_u32(&ip[12], source_ip); write_u32(&ip[16], destination_ip);
    write_u16(&ip[10], checksum(ip, IPV4_HEADER));
    uint8_t *udp = &ip[IPV4_HEADER];
    write_u16(&udp[0], source_port); write_u16(&udp[2], destination_port);
    write_u16(&udp[4], UDP_HEADER + payload_length); write_u16(&udp[6], 0);
    for (uint16_t index = 0; index < payload_length; ++index) udp[UDP_HEADER + index] = payload[index];
    packet->length = ETHERNET_HEADER + IPV4_HEADER + UDP_HEADER + payload_length;
    return 1;
}

int kernel_network_test(void) {
    NetworkBuffer packet;
    const uint8_t payload[] = { 'M', 'A', 'R', 'C', 'U', 'S' };
    if (!build_udp_packet(&packet, 0xc0a80002, 0xc0a80001, 4000, 4242, payload, sizeof(payload))) return 0;
    const uint8_t *ip = &packet.bytes[ETHERNET_HEADER];
    const uint8_t *udp = &ip[IPV4_HEADER];
    return packet.length == 48 && read_u16(&packet.bytes[12]) == 0x0800 &&
        (ip[0] >> 4) == 4 && ip[9] == 17 && read_u32(&ip[16]) == 0xc0a80001 &&
        read_u16(&udp[2]) == 4242 && udp[UDP_HEADER] == 'M' &&
        checksum(ip, IPV4_HEADER) == 0;
}

static void pci_outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static uint32_t pci_inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static uint32_t pci_read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    uint32_t address = 0x80000000u | ((uint32_t)bus << 16) |
        ((uint32_t)slot << 11) | ((uint32_t)function << 8) | (offset & 0xfcu);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    return pci_inl(PCI_CONFIG_DATA);
}

static void pci_write(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = 0x80000000u | ((uint32_t)bus << 16) |
        ((uint32_t)slot << 11) | ((uint32_t)function << 8) | (offset & 0xfcu);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    pci_outl(PCI_CONFIG_DATA, value);
}

static void pci_enable_bus_master(void) {
    uint32_t command = pci_read(e1000_pci_bus, e1000_pci_slot, 0, 0x04);
    pci_write(e1000_pci_bus, e1000_pci_slot, 0, 0x04, (command & 0xffff0000u) | ((command | 0x0006u) & 0xffffu));
}

static int e1000_probe(void) {
    for (uint8_t bus = 0; bus < 4; ++bus) {
        for (uint8_t slot = 0; slot < 32; ++slot) {
            uint32_t identity = pci_read(bus, slot, 0, 0);
            uint16_t vendor = (uint16_t)(identity & 0xffff);
            uint16_t device = (uint16_t)(identity >> 16);
            if (vendor == E1000_VENDOR && device != 0xffff) {
                for (uint8_t bar_index = 0; bar_index < 6; ++bar_index) {
                    uint8_t bar_offset = (uint8_t)(0x10 + bar_index * 4);
                    uint32_t bar = pci_read(bus, slot, 0, bar_offset);
                    if (bar == 0 || (bar & 0x01) != 0) continue;
                    uint64_t address = (uint64_t)(bar & 0xfffffff0u);
                    if ((bar & 0x06) == 0x04 && bar_index < 5) {
                        address |= (uint64_t)pci_read(bus, slot, 0, (uint8_t)(bar_offset + 4)) << 32;
                    }
                    if (address <= 0x0000fffffffff000ULL) {
                        e1000_mmio_base = address;
                        e1000_device = device;
                        e1000_pci_bus = bus;
                        e1000_pci_slot = slot;
                        e1000_present = 1;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

static volatile uint32_t *e1000_register(uint32_t offset) {
    return (volatile uint32_t *)(e1000_virtual_base + offset);
}

static void e1000_delay(void) {
    for (volatile uint32_t attempt = 0; attempt < 100000; ++attempt) {
        (void)*e1000_register(E1000_STATUS);
    }
}

static int e1000_init_mmio(void) {
    if (!e1000_present || e1000_mmio_base == 0) { e1000_init_stage = 1; return 0; }
    if ((e1000_mmio_base & 0xfff) != 0) { e1000_init_stage = 2; return 0; }
    pci_enable_bus_master();
    if (!kernel_mmio_map(E1000_VIRTUAL_BASE, (uintptr_t)e1000_mmio_base, 16)) { e1000_init_stage = 3; return 0; }
    e1000_virtual_base = E1000_VIRTUAL_BASE;
    *e1000_register(E1000_IMC) = 0xffffffffu;
    *e1000_register(E1000_CTRL) |= E1000_CTRL_RST;
    e1000_delay();
    if (*e1000_register(E1000_STATUS) == 0xffffffffu) { e1000_init_stage = 5; return 0; }
    *e1000_register(E1000_CTRL) |= E1000_CTRL_SLU;
    e1000_delay();
    uint32_t ral = *e1000_register(E1000_RAL);
    uint32_t rah = *e1000_register(E1000_RAH);
    if (ral == 0 && (rah & 0xffffu) == 0) {
        ral = 0x12005452u;
        rah = 0x80005634u;
        *e1000_register(E1000_RAL) = ral;
        *e1000_register(E1000_RAH) = rah;
    } else {
        *e1000_register(E1000_RAH) = rah | 0x80000000u;
    }
    local_mac[0] = (uint8_t)ral;
    local_mac[1] = (uint8_t)(ral >> 8);
    local_mac[2] = (uint8_t)(ral >> 16);
    local_mac[3] = (uint8_t)(ral >> 24);
    local_mac[4] = (uint8_t)rah;
    local_mac[5] = (uint8_t)(rah >> 8);
    for (uint8_t index = 0; index < 128; ++index) {
        *e1000_register(E1000_MTA + index * 4) = 0;
    }
    e1000_init_stage = 6;
    return 1;
}

static void e1000_write_ring(uint32_t register_offset, uintptr_t address, uint32_t length) {
    *e1000_register(register_offset) = (uint32_t)address;
    *e1000_register(register_offset + 4) = (uint32_t)(address >> 32);
    *e1000_register(register_offset + 8) = length;
}

static int e1000_init_rings(void) {
    if (!e1000_present || e1000_virtual_base == 0) { e1000_ring_stage = 1; return 0; }
    tx_next = 0;
    rx_next = 0;
    for (uint8_t index = 0; index < E1000_RING_SIZE; ++index) {
        rx_ring[index].buffer_address = (uint64_t)(uintptr_t)&rx_buffers[index][0];
        rx_ring[index].length = 0;
        rx_ring[index].checksum = 0;
        rx_ring[index].status = 0;
        rx_ring[index].errors = 0;
        rx_ring[index].special = 0;
        tx_ring[index].buffer_address = (uint64_t)(uintptr_t)&tx_buffers[index][0];
        tx_ring[index].length = 0;
        tx_ring[index].checksum_offset = 0;
        tx_ring[index].command = 0;
        tx_ring[index].status = E1000_DESC_DD;
        tx_ring[index].checksum_start = 0;
        tx_ring[index].special = 0;
    }
    e1000_write_ring(E1000_RDBAL, (uintptr_t)rx_ring, E1000_RING_BYTES);
    *e1000_register(E1000_RDH) = 0;
    *e1000_register(E1000_RDT) = E1000_RING_SIZE - 1;
    *e1000_register(E1000_RCTL) = E1000_RCTL_EN | E1000_RCTL_BAM | E1000_RCTL_SECRC;
    *e1000_register(E1000_RXDCTL) = 0x01000000u;
    e1000_write_ring(E1000_TDBAL, (uintptr_t)tx_ring, E1000_RING_BYTES);
    *e1000_register(E1000_TDH) = 0;
    *e1000_register(E1000_TDT) = 0;
    *e1000_register(E1000_TXDCTL) = 0x01000000u;
    *e1000_register(E1000_TCTL) = E1000_TCTL_EN | E1000_TCTL_PSP | (0x0fu << 4) | (0x40u << 12);
    *e1000_register(E1000_TIPG) = 0x0060200au;
    *e1000_register(E1000_IMS) = 0x00000084u;
    if (*e1000_register(E1000_RDBAL + 8) != E1000_RING_BYTES ||
        *e1000_register(E1000_TDBAL + 8) != E1000_RING_BYTES) {
        e1000_ring_stage = 2;
        return 0;
    }
    e1000_ring_stage = 3;
    return 1;
}

static int e1000_transmit(const uint8_t *packet, uint16_t length) {
    if (length == 0 || length > 1514) return 0;
    uint8_t index = tx_next;
    copy_bytes(tx_buffers[index], packet, length);
    tx_ring[index].buffer_address = (uint64_t)(uintptr_t)&tx_buffers[index][0];
    tx_ring[index].length = length;
    tx_ring[index].checksum_offset = 0;
    tx_ring[index].checksum_start = 0;
    tx_ring[index].special = 0;
    tx_ring[index].status = 0;
    __asm__ volatile ("sfence" : : : "memory");
    tx_ring[index].command = E1000_TX_CMD_EOP | E1000_TX_CMD_IFCS | E1000_TX_CMD_RS;
    tx_next = (uint8_t)((index + 1) % E1000_RING_SIZE);
    e1000_last_tdh = *e1000_register(E1000_TDH);
    e1000_last_tdt = *e1000_register(E1000_TDT);
    e1000_last_txdctl = *e1000_register(E1000_TXDCTL);
    e1000_last_tctl = *e1000_register(E1000_TCTL);
    *e1000_register(E1000_TDT) = tx_next;
    for (uint32_t attempt = 0; attempt < 1000000; ++attempt) {
        if ((tx_ring[index].status & E1000_DESC_DD) != 0) {
            e1000_last_tdh = *e1000_register(E1000_TDH);
            e1000_last_tdt = *e1000_register(E1000_TDT);
            e1000_last_icr = *e1000_register(E1000_ICR);
            return 1;
        }
    }
    e1000_last_icr = *e1000_register(E1000_ICR);
    return 0;
}

static int e1000_receive(uint8_t *packet, uint16_t capacity, uint16_t *length) {
    if ((rx_ring[rx_next].status & E1000_DESC_DD) == 0) return 0;
    uint16_t received = rx_ring[rx_next].length;
    if (received > capacity) received = capacity;
    copy_bytes(packet, rx_buffers[rx_next], received);
    *length = received;
    rx_ring[rx_next].status = 0;
    uint8_t previous = rx_next;
    rx_next = (uint8_t)((rx_next + 1) % E1000_RING_SIZE);
    *e1000_register(E1000_RDT) = previous;
    return 1;
}

int kernel_nic_ring_test(void) {
    if (!e1000_init_rings()) return 0;
    uint8_t packet[64];
    for (uint8_t index = 0; index < 6; ++index) packet[index] = 0xff;
    copy_bytes(&packet[6], local_mac, 6);
    write_u16(&packet[12], 0x0800);
    packet[14] = 'M'; packet[15] = 'A'; packet[16] = 'R'; packet[17] = 'C'; packet[18] = 'U'; packet[19] = 'S';
    for (uint8_t index = 20; index < 64; ++index) packet[index] = 0;
    if (!e1000_transmit(packet, 64)) {
        e1000_ring_stage = 4;
        return 0;
    }
    e1000_ring_stage = 3;
    return 1;
}

const char *kernel_nic_ring_stage_message(void) {
    static const char *messages[] = { "not initialized", "MMIO missing", "ring length rejected", "rings ready", "TX completion timeout" };
    return messages[e1000_ring_stage <= 4 ? e1000_ring_stage : 0];
}

static int arp_insert(uint32_t ip, const uint8_t *mac) {
    for (uint8_t index = 0; index < 8; ++index) {
        if (!arp_cache[index].valid || arp_cache[index].ip == ip) {
            arp_cache[index].ip = ip;
            copy_bytes(arp_cache[index].mac, mac, 6);
            arp_cache[index].valid = 1;
            return 1;
        }
    }
    return 0;
}

static int arp_lookup(uint32_t ip, uint8_t *mac) {
    for (uint8_t index = 0; index < 8; ++index) {
        if (arp_cache[index].valid && arp_cache[index].ip == ip) {
            copy_bytes(mac, arp_cache[index].mac, 6);
            return 1;
        }
    }
    return 0;
}

static void build_arp_request(uint8_t *packet) {
    for (uint8_t index = 0; index < 6; ++index) packet[index] = 0xff;
    copy_bytes(&packet[6], local_mac, 6);
    write_u16(&packet[12], 0x0806);
    write_u16(&packet[14], 1);
    write_u16(&packet[16], 0x0800);
    packet[18] = 6;
    packet[19] = 4;
    write_u16(&packet[20], 1);
    copy_bytes(&packet[22], local_mac, 6);
    write_u32(&packet[28], GUEST_IP);
    for (uint8_t index = 0; index < 6; ++index) packet[32 + index] = 0;
    write_u32(&packet[38], GATEWAY_IP);
}

static int parse_arp_reply(const uint8_t *packet, uint16_t length) {
    if (length < ARP_PACKET) return 0;
    if (read_u16(&packet[12]) != 0x0806) return 0;
    if (read_u16(&packet[20]) != 2) return 0;
    if (read_u32(&packet[28]) != GATEWAY_IP) return 0;
    if (read_u32(&packet[38]) != GUEST_IP) return 0;
    return arp_insert(GATEWAY_IP, &packet[22]);
}

static int socket_bind(uint16_t port) {
    for (uint8_t index = 0; index < 8; ++index) {
        if (!sockets[index].active) {
            sockets[index].local_port = port;
            sockets[index].remote_port = 0;
            sockets[index].remote_ip = 0;
            sockets[index].active = 1;
            return 1;
        }
    }
    return 0;
}

int kernel_nic_probe_test(void) {
    if (!e1000_probe()) { e1000_init_stage = 0; return 0; }
    return e1000_init_mmio();
}

int kernel_nic_init_stage(void) {
    return e1000_init_stage;
}

const char *kernel_nic_stage_message(void) {
    static const char *messages[] = {
        "probe failed", "BAR missing", "BAR unaligned", "MMIO map failed",
        "reset failed", "STATUS invalid", "ready"
    };
    return messages[e1000_init_stage <= 6 ? e1000_init_stage : 0];
}

int kernel_network_state_test(void) {
    const uint8_t gateway_mac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x01 };
    for (uint8_t index = 0; index < 8; ++index) {
        arp_cache[index].valid = 0;
        sockets[index].active = 0;
    }
    return arp_insert(0xc0a80001, gateway_mac) && socket_bind(4242) &&
        arp_cache[0].valid && sockets[0].local_port == 4242;
}

int kernel_nic_dma_layout_test(void) {
    uintptr_t ring_address = (uintptr_t)tx_ring;
    uintptr_t buffer_address = (uintptr_t)&tx_buffers[0][0];
    return (ring_address % 16) == 0 && (buffer_address % 16) == 0 &&
        ring_address < 0x02000000ULL && buffer_address < 0x02000000ULL;
}

const char *kernel_nic_register_state(void) {
    static const char *ready = "TDH/TDT/TXDCTL/TCTL/ICR captured";
    return ready;
}

int kernel_network_wire_test(void) {
    uint8_t request[64];
    uint8_t reply[2048];
    uint16_t reply_length = 0;
    uint8_t gateway_mac[6];
    if (!e1000_present || e1000_virtual_base == 0 || e1000_ring_stage != 3) {
        e1000_wire_stage = 1;
        return 0;
    }
    for (uint8_t index = 0; index < 8; ++index) arp_cache[index].valid = 0;
    for (uint8_t index = 0; index < 64; ++index) request[index] = 0;
    build_arp_request(request);
    if (!e1000_transmit(request, 60)) {
        e1000_wire_stage = 2;
        return 0;
    }
    for (uint32_t attempt = 0; attempt < 2000000; ++attempt) {
        if (e1000_receive(reply, sizeof(reply), &reply_length)) {
            if (parse_arp_reply(reply, reply_length)) {
                if (arp_lookup(GATEWAY_IP, gateway_mac) &&
                    (gateway_mac[0] | gateway_mac[1] | gateway_mac[2] | gateway_mac[3] | gateway_mac[4] | gateway_mac[5]) != 0) {
                    e1000_wire_stage = 5;
                    return 1;
                }
            }
        }
    }
    e1000_wire_stage = 3;
    return 0;
}

const char *kernel_nic_wire_stage_message(void) {
    static const char *messages[] = {
        "idle", "rings not ready", "ARP TX timeout", "ARP RX timeout", "bad ARP", "ARP resolved"
    };
    return messages[e1000_wire_stage <= 5 ? e1000_wire_stage : 0];
}

void kernel_nic_irq(void) {
    if (e1000_virtual_base == 0) return;
    e1000_last_icr = *e1000_register(E1000_ICR);
    ++e1000_irq_count;
}

int kernel_nic_irq_seen(void) {
    return e1000_irq_count != 0;
}

#include <stdint.h>

#define ATA_DATA 0x1f0
#define ATA_SECTOR_COUNT 0x1f2
#define ATA_LBA_LOW 0x1f3
#define ATA_LBA_MID 0x1f4
#define ATA_LBA_HIGH 0x1f5
#define ATA_DRIVE 0x1f6
#define ATA_STATUS 0x1f7
#define ATA_COMMAND 0x1f7
#define ATA_CONTROL 0x3f6

static void outb(uint16_t port, uint8_t value) { __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port)); }
static uint8_t inb(uint16_t port) { uint8_t value; __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port)); return value; }
static uint16_t inw(uint16_t port) { uint16_t value; __asm__ volatile ("inw %1, %0" : "=a"(value) : "Nd"(port)); return value; }
static void outw(uint16_t port, uint16_t value) { __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port)); }
static uint8_t block_cache[512] __attribute__((aligned(2)));
static uint32_t cached_lba;
static uint8_t cache_valid;

static int ata_wait(uint8_t required) {
    for (uint32_t attempt = 0; attempt < 1000000; ++attempt) {
        uint8_t status = inb(ATA_STATUS);
        if ((status & 0x01) != 0 || (status & 0x20) != 0) return 0;
        if ((status & required) == required && (status & 0x80) == 0) return 1;
    }
    return 0;
}

int kernel_ata_read_sector(uint32_t lba, void *buffer) {
    outb(ATA_CONTROL, 0x00);
    outb(ATA_DRIVE, (uint8_t)(0xe0 | ((lba >> 24) & 0x0f)));
    outb(ATA_SECTOR_COUNT, 1); outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8)); outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, 0x20);
    if (!ata_wait(0x08)) return 0;
    uint16_t *words = (uint16_t *)buffer;
    for (uint16_t index = 0; index < 256; ++index) words[index] = inw(ATA_DATA);
    return 1;
}

int kernel_ata_write_sector(uint32_t lba, const void *buffer) {
    outb(ATA_CONTROL, 0x00);
    outb(ATA_DRIVE, (uint8_t)(0xe0 | ((lba >> 24) & 0x0f)));
    outb(ATA_SECTOR_COUNT, 1); outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8)); outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, 0x30);
    if (!ata_wait(0x08)) return 0;
    const uint16_t *words = (const uint16_t *)buffer;
    for (uint16_t index = 0; index < 256; ++index) outw(ATA_DATA, words[index]);
    outb(ATA_COMMAND, 0xe7);
    return ata_wait(0x00);
}

int kernel_block_read(uint32_t lba, void *buffer) {
    if (cache_valid && cached_lba == lba) {
        uint8_t *destination = (uint8_t *)buffer;
        for (uint16_t index = 0; index < 512; ++index) destination[index] = block_cache[index];
        return 1;
    }
    if (!kernel_ata_read_sector(lba, block_cache)) return 0;
    cached_lba = lba;
    cache_valid = 1;
    uint8_t *destination = (uint8_t *)buffer;
    for (uint16_t index = 0; index < 512; ++index) destination[index] = block_cache[index];
    return 1;
}

int kernel_block_write(uint32_t lba, const void *buffer) {
    if (!kernel_ata_write_sector(lba, buffer)) return 0;
    cached_lba = lba;
    cache_valid = 1;
    const uint8_t *source = (const uint8_t *)buffer;
    for (uint16_t index = 0; index < 512; ++index) block_cache[index] = source[index];
    return 1;
}
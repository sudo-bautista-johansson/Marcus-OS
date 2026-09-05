#include <stdint.h>

#define PAGE_SIZE 4096ULL
#define MAX_PHYSICAL_MEMORY (128ULL * 1024ULL * 1024ULL)
#define FRAME_COUNT (MAX_PHYSICAL_MEMORY / PAGE_SIZE)
#define BITMAP_BYTES (FRAME_COUNT / 8)
#define RESERVED_END 0x200000ULL
#define PAGE_TABLE_BASE 0xc000ULL
#define TEST_VIRTUAL_ADDRESS 0x400000ULL
#define VIRTUAL_HEAP_BASE 0x400000ULL
#define VIRTUAL_HEAP_PAGES 512ULL
#define PAGE_FLAG_PRESENT 0x001ULL
#define PAGE_FLAG_WRITABLE 0x002ULL
#define PAGE_FLAG_NO_EXECUTE 0x8000000000000000ULL
#define PAGE_TABLE_POOL_COUNT 16
#define MMIO_VIRTUAL_BASE 0x2000000ULL
#define MMIO_PAGE_DIRECTORY 0xb000ULL

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} __attribute__((packed)) MemoryMapEntry;

static uint8_t frame_bitmap[BITMAP_BYTES];
static uint64_t free_frame_count;
static uintptr_t heap_frames[VIRTUAL_HEAP_PAGES];
static uint64_t page_table_pool[PAGE_TABLE_POOL_COUNT][512] __attribute__((aligned(4096)));
static uint16_t page_table_pool_next;
static uint64_t mmio_page_table[512] __attribute__((aligned(4096)));

static void mark_used(uint64_t frame) {
    frame_bitmap[frame / 8] |= (uint8_t)(1u << (frame % 8));
}

static void mark_free(uint64_t frame) {
    frame_bitmap[frame / 8] &= (uint8_t)~(1u << (frame % 8));
}

static uint8_t is_used(uint64_t frame) {
    return (uint8_t)(frame_bitmap[frame / 8] & (uint8_t)(1u << (frame % 8)));
}

void kernel_memory_init(uintptr_t map_address) {
    (void)map_address;
    for (uint64_t index = 0; index < BITMAP_BYTES; ++index) {
        frame_bitmap[index] = 0xff;
    }
    free_frame_count = 0;
    for (uint64_t index = 0; index < VIRTUAL_HEAP_PAGES; ++index) {
        heap_frames[index] = 0;
    }
    page_table_pool_next = 0;
    for (uint16_t table = 0; table < PAGE_TABLE_POOL_COUNT; ++table) {
        for (uint16_t entry = 0; entry < 512; ++entry) {
            page_table_pool[table][entry] = 0;
        }
    }

    volatile uint16_t *entry_count = (volatile uint16_t *)0x4ff0;
    volatile MemoryMapEntry *entries = (volatile MemoryMapEntry *)0x5000;
    uint16_t count = *entry_count;
    if (count > 128) {
        count = 128;
    }

    for (uint16_t index = 0; index < count; ++index) {
        uint64_t start = entries[index].base;
        uint64_t end = start + entries[index].length;
        if (entries[index].type != 1 || end <= RESERVED_END || start >= MAX_PHYSICAL_MEMORY) {
            continue;
        }
        if (start < RESERVED_END) {
            start = RESERVED_END;
        }
        if (end > MAX_PHYSICAL_MEMORY) {
            end = MAX_PHYSICAL_MEMORY;
        }
        uint64_t first_frame = (start + PAGE_SIZE - 1) / PAGE_SIZE;
        uint64_t last_frame = end / PAGE_SIZE;
        for (uint64_t frame = first_frame; frame < last_frame; ++frame) {
            if (is_used(frame)) {
                mark_free(frame);
                ++free_frame_count;
            }
        }
    }
}

uintptr_t kernel_page_alloc(void) {
    if (free_frame_count == 0) {
        return 0;
    }
    for (uint64_t frame = RESERVED_END / PAGE_SIZE; frame < FRAME_COUNT; ++frame) {
        if (!is_used(frame)) {
            mark_used(frame);
            --free_frame_count;
            return (uintptr_t)(frame * PAGE_SIZE);
        }
    }
    return 0;
}

void kernel_page_free(uintptr_t address) {
    if (address < RESERVED_END || address >= MAX_PHYSICAL_MEMORY || (address % PAGE_SIZE) != 0) {
        return;
    }
    uint64_t frame = address / PAGE_SIZE;
    if (is_used(frame)) {
        mark_free(frame);
        ++free_frame_count;
    }
}

uint64_t kernel_memory_free_frames(void) {
    return free_frame_count;
}

static void invalidate_page(uintptr_t address) {
    __asm__ volatile ("invlpg (%0)" : : "r"(address) : "memory");
}

int kernel_mmio_map(uintptr_t virtual_address, uintptr_t physical_address, uint16_t page_count) {
    if ((virtual_address % 0x200000ULL) != 0 || (physical_address % PAGE_SIZE) != 0 ||
        page_count == 0 || page_count > 512) {
        return 0;
    }
    uint64_t directory_index = virtual_address / 0x200000ULL;
    volatile uint64_t *page_directory = (volatile uint64_t *)MMIO_PAGE_DIRECTORY;
    if (directory_index >= 512 || (page_directory[directory_index] & PAGE_FLAG_PRESENT) != 0) {
        return 0;
    }
    for (uint16_t index = 0; index < 512; ++index) {
        mmio_page_table[index] = 0;
    }
    for (uint16_t index = 0; index < page_count; ++index) {
        mmio_page_table[index] = (physical_address + index * PAGE_SIZE) | PAGE_FLAG_PRESENT |
            PAGE_FLAG_WRITABLE | 0x018ULL;
        invalidate_page(virtual_address + index * PAGE_SIZE);
    }
    page_directory[directory_index] = (uint64_t)(uintptr_t)mmio_page_table | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE;
    return 1;
}

int kernel_virtual_map(uintptr_t virtual_address, uintptr_t physical_address, uint64_t flags) {
    if ((virtual_address % PAGE_SIZE) != 0 || (physical_address % PAGE_SIZE) != 0) {
        return 0;
    }
    if (virtual_address < VIRTUAL_HEAP_BASE ||
        virtual_address >= VIRTUAL_HEAP_BASE + VIRTUAL_HEAP_PAGES * PAGE_SIZE ||
        physical_address > 0x0000fffffffff000ULL) {
        return 0;
    }

    volatile uint64_t *page_directory = (volatile uint64_t *)0xb000;
    volatile uint64_t *page_table = (volatile uint64_t *)PAGE_TABLE_BASE;
    if ((page_directory[2] & 0x001) == 0) {
        for (uint16_t table_index = 0; table_index < 512; ++table_index) {
            page_table[table_index] = 0;
        }
        page_directory[2] = PAGE_TABLE_BASE | 0x003;
    }
    uint64_t index = (virtual_address - VIRTUAL_HEAP_BASE) / PAGE_SIZE;
    if ((page_table[index] & 0x001) != 0) {
        return 0;
    }
    page_table[index] = (uint64_t)physical_address | flags | PAGE_FLAG_PRESENT;
    invalidate_page(virtual_address);
    return 1;
}

int kernel_virtual_unmap(uintptr_t virtual_address) {
    if (virtual_address < VIRTUAL_HEAP_BASE ||
        virtual_address >= VIRTUAL_HEAP_BASE + VIRTUAL_HEAP_PAGES * PAGE_SIZE) {
        return 0;
    }
    volatile uint64_t *page_directory = (volatile uint64_t *)0xb000;
    volatile uint64_t *page_table = (volatile uint64_t *)PAGE_TABLE_BASE;
    uint64_t index = (virtual_address - VIRTUAL_HEAP_BASE) / PAGE_SIZE;
    page_table[index] = 0;
    invalidate_page(virtual_address);
    return 1;
}

uint64_t kernel_virtual_page_flags(uintptr_t virtual_address) {
    if (virtual_address < VIRTUAL_HEAP_BASE ||
        virtual_address >= VIRTUAL_HEAP_BASE + VIRTUAL_HEAP_PAGES * PAGE_SIZE) {
        return 0;
    }
    volatile uint64_t *page_table = (volatile uint64_t *)PAGE_TABLE_BASE;
    uint64_t index = (virtual_address - VIRTUAL_HEAP_BASE) / PAGE_SIZE;
    return page_table[index] & (PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE | PAGE_FLAG_NO_EXECUTE);
}

int kernel_virtual_mapping_test(void) {
    uintptr_t physical_address = kernel_page_alloc();
    if (physical_address == 0 || !kernel_virtual_map(TEST_VIRTUAL_ADDRESS, physical_address, 0x003)) {
        if (physical_address != 0) {
            kernel_page_free(physical_address);
        }
        return 0;
    }

    volatile uint64_t *mapped_page = (volatile uint64_t *)TEST_VIRTUAL_ADDRESS;
    mapped_page[0] = 0x4d41524355534f53ULL;
    int valid = mapped_page[0] == 0x4d41524355534f53ULL;
    kernel_virtual_unmap(TEST_VIRTUAL_ADDRESS);
    kernel_page_free(physical_address);
    return valid;
}

uintptr_t kernel_heap_alloc(uint64_t size) {
    if (size == 0 || size > VIRTUAL_HEAP_PAGES * PAGE_SIZE) {
        return 0;
    }
    uint64_t page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    volatile uint64_t *page_table = (volatile uint64_t *)PAGE_TABLE_BASE;
    for (uint64_t start = 0; start + page_count <= VIRTUAL_HEAP_PAGES; ++start) {
        uint64_t page_index = 0;
        while (page_index < page_count && (page_table[start + page_index] & 0x001) == 0) {
            ++page_index;
        }
        if (page_index != page_count) {
            start += page_index;
            continue;
        }
        for (page_index = 0; page_index < page_count; ++page_index) {
            uintptr_t physical = kernel_page_alloc();
            if (physical == 0 || !kernel_virtual_map(VIRTUAL_HEAP_BASE + (start + page_index) * PAGE_SIZE, physical, 0x003)) {
                for (uint64_t rollback = 0; rollback < page_index; ++rollback) {
                    uintptr_t rollback_virtual = VIRTUAL_HEAP_BASE + (start + rollback) * PAGE_SIZE;
                    uintptr_t rollback_physical = heap_frames[start + rollback];
                    kernel_virtual_unmap(rollback_virtual);
                    kernel_page_free(rollback_physical);
                    heap_frames[start + rollback] = 0;
                }
                if (physical != 0) {
                    kernel_page_free(physical);
                }
                return 0;
            }
            heap_frames[start + page_index] = physical;
        }
        return (uintptr_t)(VIRTUAL_HEAP_BASE + start * PAGE_SIZE);
    }
    return 0;
}

void kernel_heap_free(uintptr_t address, uint64_t size) {
    if (address < VIRTUAL_HEAP_BASE || address >= VIRTUAL_HEAP_BASE + VIRTUAL_HEAP_PAGES * PAGE_SIZE || size == 0) {
        return;
    }
    uint64_t start = (address - VIRTUAL_HEAP_BASE) / PAGE_SIZE;
    uint64_t page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (start + page_count > VIRTUAL_HEAP_PAGES) {
        return;
    }
    for (uint64_t index = 0; index < page_count; ++index) {
        uintptr_t physical = heap_frames[start + index];
        if (physical != 0) {
            kernel_virtual_unmap(address + index * PAGE_SIZE);
            kernel_page_free(physical);
            heap_frames[start + index] = 0;
        }
    }
}

int kernel_virtual_protection_test(void) {
    uintptr_t physical_address = kernel_page_alloc();
    if (physical_address == 0 || !kernel_virtual_map(TEST_VIRTUAL_ADDRESS, physical_address, PAGE_FLAG_NO_EXECUTE)) {
        if (physical_address != 0) {
            kernel_page_free(physical_address);
        }
        return 0;
    }
    uint64_t flags = kernel_virtual_page_flags(TEST_VIRTUAL_ADDRESS);
    int protected_page = (flags & PAGE_FLAG_PRESENT) != 0 &&
        (flags & PAGE_FLAG_WRITABLE) == 0 &&
        (flags & PAGE_FLAG_NO_EXECUTE) != 0;
    kernel_virtual_unmap(TEST_VIRTUAL_ADDRESS);
    kernel_page_free(physical_address);
    return protected_page;
}

static uintptr_t kernel_page_table_alloc(void) {
    if (page_table_pool_next >= PAGE_TABLE_POOL_COUNT) {
        return 0;
    }
    uintptr_t table = (uintptr_t)&page_table_pool[page_table_pool_next][0];
    ++page_table_pool_next;
    return table;
}

static uintptr_t kernel_read_cr3(void) {
    uintptr_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

static void kernel_write_cr3(uintptr_t value) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(value) : "memory");
}

uintptr_t kernel_address_space_create(void) {
    uintptr_t address_space = kernel_page_table_alloc();
    if (address_space == 0 || address_space >= 0x200000ULL) {
        return 0;
    }
    volatile uint64_t *source = (volatile uint64_t *)0x9000;
    volatile uint64_t *destination = (volatile uint64_t *)address_space;
    for (uint16_t index = 0; index < 512; ++index) {
        destination[index] = source[index];
    }
    return address_space;
}

void kernel_address_space_switch(uintptr_t address_space) {
    if (address_space != 0 && (address_space % PAGE_SIZE) == 0 && address_space < 0x200000ULL) {
        kernel_write_cr3(address_space);
    }
}

int kernel_address_space_test(void) {
    uintptr_t original = kernel_read_cr3();
    uintptr_t isolated = kernel_address_space_create();
    if (isolated == 0 || isolated == original) {
        return 0;
    }
    kernel_address_space_switch(isolated);
    uintptr_t active = kernel_read_cr3();
    kernel_address_space_switch(original);
    return active == isolated && kernel_read_cr3() == original;
}
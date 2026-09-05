#include <stdint.h>

extern int kernel_block_read(uint32_t lba, void *buffer);
extern int kernel_block_write(uint32_t lba, const void *buffer);

typedef struct {
    char name[24];
    uint32_t start_lba;
    uint32_t size;
    uint32_t flags;
} __attribute__((packed)) MarcusFile;

static uint8_t superblock[512] __attribute__((aligned(2)));
static uint8_t directory_block[512] __attribute__((aligned(2)));
static uint8_t file_block[2048] __attribute__((aligned(2)));

static void clear_bytes(uint8_t *buffer, uint16_t count) {
    for (uint16_t index = 0; index < count; ++index) buffer[index] = 0;
}

static int names_equal(const char *left, const char *right) {
    for (uint8_t index = 0; index < 24; ++index) {
        if (left[index] != right[index]) return 0;
        if (left[index] == '\0') return 1;
    }
    return 1;
}

static MarcusFile *find_file(const char *name) {
    MarcusFile *files = (MarcusFile *)directory_block;
    for (uint8_t index = 0; index < 16; ++index) {
        if (files[index].name[0] != '\0' && names_equal(files[index].name, name)) return &files[index];
    }
    return (MarcusFile *)0;
}

static MarcusFile *find_free_file(void) {
    MarcusFile *files = (MarcusFile *)directory_block;
    for (uint8_t index = 0; index < 16; ++index) {
        if (files[index].name[0] == '\0') return &files[index];
    }
    return (MarcusFile *)0;
}

static int filesystem_load_directory(void) {
    return kernel_block_read(130, directory_block);
}

static int filesystem_flush_directory(void) {
    return kernel_block_write(130, directory_block);
}

static int filesystem_read_file(const char *name, uint8_t *buffer, uint32_t capacity, uint32_t *size) {
    MarcusFile *file = find_file(name);
    if (file == (MarcusFile *)0 || (file->flags & 0x01) == 0 || file->size > capacity) return 0;
    uint32_t sectors = (file->size + 511) / 512;
    for (uint32_t sector = 0; sector < sectors; ++sector) {
        if (!kernel_block_read(file->start_lba + sector, file_block + sector * 512)) return 0;
    }
    for (uint32_t index = 0; index < file->size; ++index) buffer[index] = file_block[index];
    *size = file->size;
    return 1;
}

static int filesystem_create_file(const char *name, const uint8_t *data, uint32_t size) {
    if (size == 0 || size > sizeof(file_block) || find_file(name) != (MarcusFile *)0) return 0;
    MarcusFile *file = find_free_file();
    if (file == (MarcusFile *)0) return 0;
    clear_bytes(file_block, sizeof(file_block));
    for (uint32_t index = 0; index < size; ++index) file_block[index] = data[index];
    uint32_t sectors = (size + 511) / 512;
    for (uint32_t sector = 0; sector < sectors; ++sector) {
        if (!kernel_block_write(133 + sector, file_block + sector * 512)) return 0;
    }
    clear_bytes((uint8_t *)file, sizeof(MarcusFile));
    for (uint8_t index = 0; index < 24 && name[index] != '\0'; ++index) file->name[index] = name[index];
    file->start_lba = 133;
    file->size = size;
    file->flags = 0x03;
    return filesystem_flush_directory();
}

static int filesystem_delete_file(const char *name) {
    MarcusFile *file = find_file(name);
    if (file == (MarcusFile *)0 || (file->flags & 0x02) == 0) return 0;
    clear_bytes((uint8_t *)file, sizeof(MarcusFile));
    return filesystem_flush_directory();
}

int kernel_filesystem_test(void) {
    if (!kernel_block_read(129, superblock)) return 0;
    if (superblock[0] != 'M' || superblock[1] != 'F' || superblock[2] != 'S' || superblock[3] != '1') return 0;
    if (!filesystem_load_directory()) return 0;
    uint8_t welcome[512];
    uint32_t welcome_size = 0;
    if (!filesystem_read_file("WELCOME.TXT", welcome, sizeof(welcome), &welcome_size)) return 0;
    if (welcome_size < 4 || welcome[0] != 'M' || welcome[1] != 'A' || welcome[2] != 'R' || welcome[3] != 'C') return 0;
    uint8_t runtime_data[600];
    for (uint16_t index = 0; index < sizeof(runtime_data); ++index) runtime_data[index] = (uint8_t)('A' + (index % 26));
    runtime_data[0] = 'M'; runtime_data[1] = 'A'; runtime_data[2] = 'R'; runtime_data[3] = 'C';
    if (!filesystem_create_file("RUNTIME.TXT", runtime_data, sizeof(runtime_data))) return 0;
    if (!filesystem_load_directory()) return 0;
    uint8_t runtime[600];
    uint32_t runtime_size = 0;
    if (!filesystem_read_file("RUNTIME.TXT", runtime, sizeof(runtime), &runtime_size)) return 0;
    if (runtime_size != sizeof(runtime_data) || runtime[0] != 'M' || runtime[599] != runtime_data[599]) return 0;
    if (!filesystem_delete_file("RUNTIME.TXT")) return 0;
    if (!filesystem_load_directory()) return 0;
    return find_file("RUNTIME.TXT") == (MarcusFile *)0;
}
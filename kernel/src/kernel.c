#include <stdint.h>

static volatile uint16_t *const vga = (uint16_t *)0xb8000;
static uint16_t cursor;

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void serial_init(void) {
    outb(0x3f9, 0x00);
    outb(0x3fb, 0x80);
    outb(0x3f8, 0x03);
    outb(0x3f9, 0x00);
    outb(0x3fb, 0x03);
    outb(0x3fa, 0xc7);
    outb(0x3fc, 0x0b);
}

static void put_char(char value) {
    if (value == '\n') {
        cursor = (uint16_t)(((cursor / 80) + 1) * 80);
        outb(0x3f8, '\r');
        outb(0x3f8, '\n');
    } else {
        vga[cursor++] = (uint16_t)value | 0x0f00;
        outb(0x3f8, (uint8_t)value);
    }
    if (cursor >= 80 * 25) {
        cursor = 0;
    }
}

void kernel_puts(const char *text) {
    serial_init();
    while (*text != '\0') {
        put_char(*text++);
    }
}


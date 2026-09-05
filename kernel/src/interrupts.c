#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) IdtEntry;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IdtPointer;

extern void marcus_exception_stub(void);
extern void marcus_irq0_stub(void);
extern void marcus_irq1_stub(void);
extern void marcus_irq11_stub(void);
extern void kernel_scheduler_tick(void);
extern void kernel_nic_irq(void);

static IdtEntry idt[256] __attribute__((aligned(16)));
static volatile uint64_t timer_ticks;
static volatile uint8_t last_scancode;

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void set_idt_handler(uint8_t vector, uintptr_t handler) {
    idt[vector].offset_low = (uint16_t)(handler & 0xffff);
    idt[vector].selector = 0x18;
    idt[vector].ist = 0;
    idt[vector].attributes = 0x8e;
    idt[vector].offset_mid = (uint16_t)((handler >> 16) & 0xffff);
    idt[vector].offset_high = (uint32_t)(handler >> 32);
    idt[vector].reserved = 0;
}

void kernel_idt_init(void) {
    uintptr_t handler = (uintptr_t)marcus_exception_stub;
    for (uint16_t index = 0; index < 256; ++index) {
        idt[index].offset_low = (uint16_t)(handler & 0xffff);
        idt[index].selector = 0x18;
        idt[index].ist = 0;
        idt[index].attributes = 0x8e;
        idt[index].offset_mid = (uint16_t)((handler >> 16) & 0xffff);
        idt[index].offset_high = (uint32_t)(handler >> 32);
        idt[index].reserved = 0;
    }

    IdtPointer pointer = {
        .limit = (uint16_t)(sizeof(idt) - 1),
        .base = (uint64_t)(uintptr_t)idt,
    };
    __asm__ volatile ("lidt %0" : : "m"(pointer));
    set_idt_handler(32, (uintptr_t)marcus_irq0_stub);
    set_idt_handler(33, (uintptr_t)marcus_irq1_stub);
    set_idt_handler(43, (uintptr_t)marcus_irq11_stub);
}

void kernel_interrupts_init(void) {
    outb(0x20, 0x11);
    outb(0xa0, 0x11);
    outb(0x21, 0x20);
    outb(0xa1, 0x28);
    outb(0x21, 0x04);
    outb(0xa1, 0x02);
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
    outb(0x21, 0xf8);
    outb(0xa1, 0xf7);

    uint16_t divisor = 1193182 / 100;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xff));
    outb(0x40, (uint8_t)(divisor >> 8));
    __asm__ volatile ("sti");
}

void kernel_timer_tick(void) {
    ++timer_ticks;
    kernel_scheduler_tick();
}

void kernel_keyboard_irq(void) {
    last_scancode = inb(0x60);
}

void kernel_irq_dispatch(uint8_t vector) {
    if (vector == 32) {
        kernel_timer_tick();
    } else if (vector == 33) {
        kernel_keyboard_irq();
    } else if (vector == 43) {
        kernel_nic_irq();
    }
    if (vector >= 40) {
        outb(0xa0, 0x20);
    }
    if (vector >= 32 && vector < 48) {
        outb(0x20, 0x20);
    }
}

int kernel_wait_for_tick(void) {
    uint64_t before = timer_ticks;
    while (timer_ticks == before) {
        __asm__ volatile ("hlt");
    }
    return 1;
}

void kernel_exception_panic(void) {
    const char *message = "MARCUS OS: CPU exception\n";
    while (*message != '\0') {
        __asm__ volatile ("outb %0, %1" : : "a"((uint8_t)*message++), "Nd"((uint16_t)0x3f8));
    }
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

void kernel_halt(void) {
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
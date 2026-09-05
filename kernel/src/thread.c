#include <stdint.h>

typedef struct {
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
    uint64_t rflags;
} CpuContext;

extern void kernel_context_switch(CpuContext *from, CpuContext *to);

static CpuContext main_context;
static CpuContext worker_context;
static uint8_t worker_stack[4096] __attribute__((aligned(16)));
static volatile uint64_t worker_value;

static void worker_entry(void) {
    worker_value = 0x4d41524355535448ULL;
    kernel_context_switch(&worker_context, &main_context);
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

int kernel_context_switch_test(void) {
    worker_value = 0;
    worker_context.rsp = ((uintptr_t)worker_stack + sizeof(worker_stack) - 8) & ~0x0fULL;
    worker_context.rip = (uintptr_t)worker_entry;
    worker_context.rflags = 0x202;
    kernel_context_switch(&main_context, &worker_context);
    return worker_value == 0x4d41524355535448ULL;
}
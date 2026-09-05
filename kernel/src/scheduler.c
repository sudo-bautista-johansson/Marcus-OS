#include <stdint.h>

#define MAX_RUNNABLE_THREADS 16

typedef struct {
    uint64_t id;
    uint8_t runnable;
    uint64_t time_slices;
} KernelThread;

static KernelThread run_queue[MAX_RUNNABLE_THREADS];
static uint8_t run_queue_length;
static uint8_t current_thread;

void kernel_scheduler_init(void) {
    run_queue_length = 0;
    current_thread = 0;
    for (uint8_t index = 0; index < MAX_RUNNABLE_THREADS; ++index) {
        run_queue[index].id = 0;
        run_queue[index].runnable = 0;
        run_queue[index].time_slices = 0;
    }
}

int kernel_scheduler_add(uint64_t id) {
    if (run_queue_length >= MAX_RUNNABLE_THREADS || id == 0) {
        return 0;
    }
    run_queue[run_queue_length].id = id;
    run_queue[run_queue_length].runnable = 1;
    run_queue[run_queue_length].time_slices = 0;
    ++run_queue_length;
    return 1;
}

void kernel_scheduler_tick(void) {
    if (run_queue_length == 0) {
        return;
    }
    run_queue[current_thread].time_slices++;
    for (uint8_t offset = 1; offset <= run_queue_length; ++offset) {
        uint8_t candidate = (uint8_t)((current_thread + offset) % run_queue_length);
        if (run_queue[candidate].runnable) {
            current_thread = candidate;
            return;
        }
    }
}

uint64_t kernel_scheduler_current(void) {
    if (run_queue_length == 0) {
        return 0;
    }
    return run_queue[current_thread].id;
}

int kernel_scheduler_test(void) {
    kernel_scheduler_init();
    if (!kernel_scheduler_add(1) || !kernel_scheduler_add(2)) {
        return 0;
    }
    if (kernel_scheduler_current() != 1) {
        return 0;
    }
    kernel_scheduler_tick();
    return kernel_scheduler_current() == 2;
}
#include <stdint.h>

#define MAX_PROCESSES 8

typedef struct {
    uint64_t pid;
    uintptr_t address_space;
    uint8_t active;
} KernelProcess;

extern uintptr_t kernel_address_space_create(void);

static KernelProcess processes[MAX_PROCESSES];
static uint64_t next_pid = 1;

int kernel_process_test(void) {
    for (uint8_t index = 0; index < MAX_PROCESSES; ++index) {
        processes[index].pid = 0;
        processes[index].address_space = 0;
        processes[index].active = 0;
    }
    uintptr_t first_space = kernel_address_space_create();
    uintptr_t second_space = kernel_address_space_create();
    if (first_space == 0 || second_space == 0 || first_space == second_space) {
        return 0;
    }
    processes[0].pid = next_pid++;
    processes[0].address_space = first_space;
    processes[0].active = 1;
    processes[1].pid = next_pid++;
    processes[1].address_space = second_space;
    processes[1].active = 1;
    return processes[0].pid != processes[1].pid &&
        processes[0].address_space != processes[1].address_space;
}
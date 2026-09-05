#include <stdint.h>

#define IPC_CAPACITY 8

typedef struct {
    uint64_t values[IPC_CAPACITY];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} IpcChannel;

static IpcChannel channel;

static int ipc_send(uint64_t value) {
    if (channel.count == IPC_CAPACITY) {
        return 0;
    }
    channel.values[channel.tail] = value;
    channel.tail = (uint8_t)((channel.tail + 1) % IPC_CAPACITY);
    ++channel.count;
    return 1;
}

static int ipc_receive(uint64_t *value) {
    if (channel.count == 0) {
        return 0;
    }
    *value = channel.values[channel.head];
    channel.head = (uint8_t)((channel.head + 1) % IPC_CAPACITY);
    --channel.count;
    return 1;
}

int kernel_ipc_test(void) {
    channel.head = 0;
    channel.tail = 0;
    channel.count = 0;
    if (!ipc_send(0x4d41524355534950ULL)) {
        return 0;
    }
    uint64_t value = 0;
    return ipc_receive(&value) && value == 0x4d41524355534950ULL && channel.count == 0;
}
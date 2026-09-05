#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
void kryo_cpuid_raw(uint32_t leaf, uint32_t subleaf, uintptr_t out) {
    int info[4];
    __cpuidex(info, (int)leaf, (int)subleaf);
    uint32_t* ptr = (uint32_t*)out;
    ptr[0] = (uint32_t)info[0]; // eax
    ptr[1] = (uint32_t)info[1]; // ebx
    ptr[2] = (uint32_t)info[2]; // ecx
    ptr[3] = (uint32_t)info[3]; // edx
}
#else
void kryo_cpuid_raw(uint32_t leaf, uint32_t subleaf, uintptr_t out) {
    uint32_t eax, ebx, ecx, edx;
    __asm__ __volatile__(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(leaf), "c"(subleaf)
    );
    uint32_t* ptr = (uint32_t*)out;
    ptr[0] = eax;
    ptr[1] = ebx;
    ptr[2] = ecx;
    ptr[3] = edx;
}
#endif

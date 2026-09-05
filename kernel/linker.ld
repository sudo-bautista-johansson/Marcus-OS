ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000;
    .text : ALIGN(16) { *(.text*) }
    .rodata : ALIGN(16) { *(.rodata*) }
    .data : ALIGN(16) { *(.data*) }
    .dma : ALIGN(4096) { *(.dma) }
    .bss : ALIGN(16) { *(COMMON) *(.bss*) }
}
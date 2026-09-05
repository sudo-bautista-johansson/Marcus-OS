.code16
.section .text,"ax"
.global _start

.set KERNEL_LOAD, 0x10000
.set KERNEL_ENTRY, 0x100000
.set PAGE_TABLES, 0x9000
.set KERNEL_SECTORS, 128

_start:
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7c00, %sp
    movb %dl, boot_drive
    movw $0x0013, %ax
    int $0x10
    call detect_memory
    movw $disk_packet, %si
    movb $0x42, %ah
    movb boot_drive, %dl
    int $0x13
    jc disk_error
    lgdt gdt_descriptor
    movl %cr0, %eax
    orl $1, %eax
    movl %eax, %cr0
    ljmp $0x08, $protected_mode

detect_memory:
    movw $0x04ff, %ax
    movw %ax, %es
    xorw %di, %di
    xorw %ax, %ax
    movw %ax, %es:(%di)
    movw $0x0500, %ax
    movw %ax, %es
    xorw %di, %di
    xorl %ebx, %ebx
    xorw %bp, %bp
1:
    movl $0xe820, %eax
    movl $0x534d4150, %edx
    movl $24, %ecx
    int $0x15
    jc 2f
    cmpl $0x534d4150, %eax
    jne 2f
    incw %bp
    addw $24, %di
    cmpw $128, %bp
    jb 1b
2:
    movw $0x04ff, %ax
    movw %ax, %es
    xorw %di, %di
    movw %bp, %es:(%di)
    ret

disk_error:
    movw $disk_error_message, %si
    call print_string
1:
    hlt
    jmp 1b

print_string:
    lodsb
    testb %al, %al
    jz 2f
    movb $0x0e, %ah
    int $0x10
    jmp print_string
2:
    ret

.code32
protected_mode:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movl $KERNEL_LOAD, %esi
    movl $KERNEL_ENTRY, %edi
    movl $(KERNEL_SECTORS * 128), %ecx
    cld
    rep movsl

    movl $PAGE_TABLES, %edi
    xorl %eax, %eax
    movl $0x300, %ecx
    rep stosl
    movl $PAGE_TABLES, %edi
    movl $(PAGE_TABLES + 0x1000 + 0x003), (%edi)
    movl $(PAGE_TABLES + 0x2000 + 0x003), 0x1000(%edi)
    movl $PAGE_TABLES, %edi
    addl $0x2000, %edi
    xorl %eax, %eax
    movl $16, %ecx
1:
    movl %eax, (%edi)
    orl $0x00000083, (%edi)
    addl $8, %edi
    addl $0x200000, %eax
    loop 1b
    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4
    movl $PAGE_TABLES, %eax
    movl %eax, %cr3
    movl $0xc0000080, %ecx
    rdmsr
    orl $0x900, %eax
    wrmsr
    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0
    ljmp $0x18, $long_mode

.code64
long_mode:
    movw $0x20, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movq $0x1200000, %rsp
    movq $KERNEL_ENTRY, %rax
    jmp *%rax

.align 8
disk_packet:
    .byte 0x10, 0x00
    .word KERNEL_SECTORS
    .word 0x0000
    .word 0x1000
    .quad 1
boot_drive:
    .byte 0
disk_error_message:
    .asciz "MARCUS OS: disk read failed\r\n"

.align 8
gdt:
    .quad 0x0000000000000000
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
    .quad 0x00af9a000000ffff
    .quad 0x00af92000000ffff
gdt_descriptor:
    .word (5 * 8) - 1
    .long gdt

.org 510
    .word 0xaa55
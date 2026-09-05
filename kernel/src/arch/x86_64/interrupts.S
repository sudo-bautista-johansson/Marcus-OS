.code64
.section .text
.global marcus_exception_stub
.type marcus_exception_stub,@function
.global marcus_irq0_stub
.global marcus_irq1_stub
.global marcus_irq11_stub

marcus_exception_stub:
    cld
    call kernel_exception_panic
1:
    hlt
    jmp 1b

.macro SAVE_REGISTERS
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
.endm

.macro RESTORE_REGISTERS
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
.endm

marcus_irq0_stub:
    SAVE_REGISTERS
    mov $32, %edi
    call kernel_irq_dispatch
    RESTORE_REGISTERS
    iretq

marcus_irq1_stub:
    SAVE_REGISTERS
    mov $33, %edi
    call kernel_irq_dispatch
    RESTORE_REGISTERS
    iretq

marcus_irq11_stub:
    SAVE_REGISTERS
    mov $43, %edi
    call kernel_irq_dispatch
    RESTORE_REGISTERS
    iretq
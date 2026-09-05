.code64
.section .text
.global kernel_context_switch
.type kernel_context_switch,@function

kernel_context_switch:
    mov %rsp, 0(%rdi)
    mov %rbp, 8(%rdi)
    mov %rbx, 16(%rdi)
    mov %r12, 24(%rdi)
    mov %r13, 32(%rdi)
    mov %r14, 40(%rdi)
    mov (%rsp), %rax
    lea 8(%rsp), %rcx
    mov %rcx, 0(%rdi)
    mov %rax, 56(%rdi)
    pushfq
    pop %rax
    mov %rax, 64(%rdi)

    mov 8(%rsi), %rbp
    mov 16(%rsi), %rbx
    mov 24(%rsi), %r12
    mov 32(%rsi), %r13
    mov 40(%rsi), %r14
    mov 0(%rsi), %rsp
    push 64(%rsi)
    popfq
    jmp *56(%rsi)
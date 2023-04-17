.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "=========="

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	pushq $0
	pushq $0
	movq $0, %rax
	movq %rax, -8(%rbp)
	movq $6, %rax
	movq %rax, -16(%rbp)
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	cmpq %r10, %rax
	movq $0, %rax
	movq %rbp, %rsp
	popq %rbp
	ret

.max:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	cmpq %r10, %rax
	movq $0, %rax
	movq %rbp, %rsp
	popq %rbp
	ret

.min:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	cmpq %r10, %rax
	movq $0, %rax
	movq %rbp, %rsp
	popq %rbp
	ret

main:
	pushq %rbp
	movq %rsp, %rbp
	subq $1, %rdi
	cmpq $0, %rdi
	jne ABORT
	call .main
	movq %rax, %rdi
	call exit
ABORT:
	leaq errout(%rip), %rdi
	call puts
	movq $1, %rdi
	call exit
safe_printf:
	pushq %rbp
	movq %rsp, %rbp
	andq $-16, %rsp
	call printf
	movq %rbp, %rsp
	popq %rbp
	ret
.global main

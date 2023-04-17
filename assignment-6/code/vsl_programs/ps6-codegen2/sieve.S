.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"

.section .bss
.align 8
.sieve: 	.zero 8000

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	pushq $0
	pushq $0
	movq $2, %rax
	movq %rax, -8(%rbp)
	movq $1000, %rax
	movq %rax, -16(%rbp)
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %rax
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
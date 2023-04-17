.section __TEXT, __cstring
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "=========="
<<<<<<< HEAD
.section __DATA, __bss
||||||| update readme
.section .bss
=======

.section .bss
>>>>>>> main
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
	popq %rax
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
	popq %rax
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
.set printf, _printf
.set putchar, _putchar
.set puts, _puts
.set strtol, _strtol
.set exit, _exit
.set _main, main
.global _main

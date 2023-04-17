.section __TEXT, __cstring
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "Fibonnacci is now calculated, now using array as a linked list!"
string1: 	.asciz "at array["
string2: 	.asciz "] we find"
string3: 	.asciz "array[array["
string4: 	.asciz "]] is "
<<<<<<< HEAD
.section __DATA, __bss
||||||| update readme
.section .bss
=======

.section .bss
>>>>>>> main
.align 8
.array: 	.zero 800

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	pushq $0
	movq $1, %rax
	pushq %rax
	movq $0, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq $2, %rax
	pushq %rax
	movq $1, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq $2, %rax
	movq %rax, -16(%rbp)
	movq $100, %rax
	movq %rax, -24(%rbp)
	movq -16(%rbp), %rax
	pushq %rax
	movq -24(%rbp), %rax
	popq %rax
	cmpq %r10, %rax
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $0, %rax
	movq %rax, -8(%rbp)
	movq $1, %rax
	pushq %rax
	movq $1, %rax
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

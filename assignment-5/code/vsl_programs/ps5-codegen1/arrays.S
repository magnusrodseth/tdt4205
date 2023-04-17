.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "The values of the array are:"

.section .bss
.align 8
.array: 	.zero 80

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	movq $1, %rax
	pushq %rax
	movq $0, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq $0, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	pushq %rax
	movq $1, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq $3, %rax
	pushq %rax
	movq $1, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $0, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $1, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $2, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
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

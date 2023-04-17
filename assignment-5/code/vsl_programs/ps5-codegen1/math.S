.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "num*2"
string1: 	.asciz "num+5"
string2: 	.asciz "num/3"
string3: 	.asciz "4*(-num)"
string4: 	.asciz "(num-1)*(num+1)*(num+3)"

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	pushq %rax
	movq $2, %rax
	popq %r10
	imulq %r10, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq strout(%rip), %rdi
	leaq string1(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	pushq %rax
	movq $5, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq strout(%rip), %rdi
	leaq string2(%rip), %rsi
	call safe_printf
	movq $3, %rax
	pushq %rax
	movq -8(%rbp), %rax
	cqo
	popq %r10
	idivq %r10
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq strout(%rip), %rdi
	leaq string3(%rip), %rsi
	call safe_printf
	movq $4, %rax
	pushq %rax
	movq -8(%rbp), %rax
	negq %rax
	popq %r10
	imulq %r10, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq strout(%rip), %rdi
	leaq string4(%rip), %rsi
	call safe_printf
	movq $1, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %r10
	subq %r10, %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	popq %r10
	imulq %r10, %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	movq $3, %rax
	popq %r10
	addq %r10, %rax
	popq %r10
	imulq %r10, %rax
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
	cmpq $1, %rdi
	jne ABORT
	addq $8, %rsi
	movq %rdi, %rcx
PARSE_ARGV:
	pushq %rsi
	pushq %rcx
	movq (%rsi), %rdi
	movq $0, %rsi
	movq $10, %rdx
	call strtol
	popq %rcx
	popq %rsi
	pushq %rax
	subq $8, %rsi
	loop PARSE_ARGV
	popq %rdi
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

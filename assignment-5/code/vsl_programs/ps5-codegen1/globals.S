.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "On startup, A and B are:"
string1: 	.asciz "Now, B is"
string2: 	.asciz "Here, A is"

.section .bss
.align 8
.A: 	.zero 8
.B: 	.zero 8
.dummy: 	.zero 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq .A(%rip), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq .B(%rip), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $5, %rax
	movq %rax, .A(%rip)
	call .otherFunc
	movq %rax, .dummy(%rip)
	leaq strout(%rip), %rdi
	leaq string1(%rip), %rsi
	call safe_printf
	movq .B(%rip), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $0, %rax
	movq %rbp, %rsp
	popq %rbp
	ret

.otherFunc:
	pushq %rbp
	movq %rsp, %rbp
	leaq strout(%rip), %rdi
	leaq string2(%rip), %rsi
	call safe_printf
	movq .A(%rip), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $2, %rax
	movq %rax, .B(%rip)
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

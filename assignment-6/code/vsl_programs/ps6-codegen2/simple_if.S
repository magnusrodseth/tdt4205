.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "You"
string1: 	.asciz "Are"
string2: 	.asciz "WRONG1"
string3: 	.asciz "WRONG2"
string4: 	.asciz "Getting"
string5: 	.asciz "WRONG3"
string6: 	.asciz "Correct"
string7: 	.asciz "WRONG4"
string8: 	.asciz "Output"

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	movq $1, %rax
	pushq %rax
	movq $1, %rax
	popq %rax
	cmpq %r10, %rax
	leaq strout(%rip), %rdi
	leaq string8(%rip), %rsi
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

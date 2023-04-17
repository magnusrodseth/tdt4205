.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "a is less than 10"
string1: 	.asciz "a is more than -5"
string2: 	.asciz "a is less than or equal to -5"
string3: 	.asciz "a is equal to 5"
string4: 	.asciz "a is not equal to 0"
string5: 	.asciz "a is however 6"
string6: 	.asciz "a is however 7"
string7: 	.asciz "a is however 8"
string8: 	.asciz "a is neither 0, 6, 7 or 8, but"

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	movq -8(%rbp), %rax
	pushq %rax
	movq $10, %rax
	popq %r10
	cmpq %r10, %rax
	movq -8(%rbp), %rax
	pushq %rax
	movq $-5, %rax
	popq %r10
	cmpq %r10, %rax
	movq -8(%rbp), %rax
	pushq %rax
	movq $5, %rax
	popq %r10
	cmpq %r10, %rax
	movq -8(%rbp), %rax
	pushq %rax
	movq $0, %rax
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

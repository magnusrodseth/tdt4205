.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "Inside main, the arguments are:"
string1: .asciz "Inside otherFunc, the arguments are:"

.section .bss
.align 8

.section .text
.main:

.otherFunc:

.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $1, %rdi
	cmpq $8, %rdi
	jne ABORT
	addq $64, %rsi
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
	popq %rsi
	popq %rdx
	popq %rcx
	popq %r8
	popq %r9
	call .main
	movq %rax, %rdi
	call exit
ABORT:
	movq $errout, %rdi
	call puts
	movq $1, %rdi
	call exit

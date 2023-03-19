.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "num*2"
string1: .asciz "num+5"
string2: .asciz "num/3"
string3: .asciz "4*(-num)"
string4: .asciz "(num-1)*(num+1)*(num+3)"

.section .bss
.align 8

.section .text
.main:

.globl main
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
	movq $errout, %rdi
	call puts
	movq $1, %rdi
	call exit

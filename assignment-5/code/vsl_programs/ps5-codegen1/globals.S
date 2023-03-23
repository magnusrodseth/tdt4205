.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "On startup, A and B are:"
string1: .asciz "Now, B is"
string2: .asciz "Here, A is"

.section .bss
.align 8
.A: .zero 8
.B: .zero 8
.dummy: .zero 8

.section .text
.main:

.otherFunc:

.globl main
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
	movq $errout, %rdi
	call puts
	movq $1, %rdi
	call exit

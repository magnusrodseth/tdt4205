.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "Now, a b c are"
string1: .asciz "In here, a b c are"
string2: .asciz "Out here, a b c are"

.section .bss
.align 8

.section .text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	pushq $0
	pushq $0
	pushq $0
	pushq $0

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

.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "The values of the array are:"

.section .bss
.align 8
.array: .zero 0

.section .text
.main:

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

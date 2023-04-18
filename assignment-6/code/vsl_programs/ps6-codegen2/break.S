.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "<="
string1: 	.asciz "===="

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
while0:
	movq $1, %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	cmpq %rax, %r10
	jne endwhile0
	movq -8(%rbp), %rax
	movq %rax, -16(%rbp)
while1:
	movq $1, %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	cmpq %rax, %r10
	jne endwhile1
	movq -16(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $1, %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	subq %r10, %rax
	movq %rax, -16(%rbp)
if0:
	movq -16(%rbp), %rax
	pushq %rax
	movq $0, %rax
	popq %r10
	cmpq %rax, %r10
	jge else0
	jmp endwhile1
	jmp endif0
else0:
endif0:
	jmp while1
endwhile1:
	leaq strout(%rip), %rdi
	leaq string1(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq -8(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -8(%rbp)
if1:
	movq -8(%rbp), %rax
	pushq %rax
	movq $5, %rax
	popq %r10
	cmpq %rax, %r10
	jne else1
	jmp endwhile0
	jmp endif1
else1:
endif1:
	jmp while0
endwhile0:
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

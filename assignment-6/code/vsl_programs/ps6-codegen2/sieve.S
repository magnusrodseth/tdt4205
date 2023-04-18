.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"

.section .bss
.align 8
.sieve: 	.zero 8000

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	pushq $0
	pushq $0
	movq $2, %rax
	movq %rax, -8(%rbp)
	movq $1000, %rax
	movq %rax, -16(%rbp)
while0:
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile0
if0:
	movq -8(%rbp), %rax
	leaq .sieve(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	pushq %rax
	movq $0, %rax
	popq %r10
	cmpq %rax, %r10
	jne else0
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $2, %rax
	movq %rax, -24(%rbp)
	movq $1000, %rax
	movq %rax, -32(%rbp)
while1:
	movq -24(%rbp), %rax
	pushq %rax
	movq -32(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile1
if1:
	movq -24(%rbp), %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %r10
	imulq %r10, %rax
	pushq %rax
	movq $999, %rax
	popq %r10
	cmpq %rax, %r10
	jle else1
	jmp endif1
else1:
endif1:
	movq $1, %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	movq -24(%rbp), %rax
	popq %r10
	imulq %r10, %rax
	leaq .sieve(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq -24(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -24(%rbp)
	jmp while1
endwhile1:
	jmp endif0
else0:
endif0:
	movq -8(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -8(%rbp)
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

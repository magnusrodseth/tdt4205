.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "First printing every power of two less than 1000"
string1: 	.asciz "i is now"
string2: 	.asciz "Now printing every product with factors less than 6"
string3: 	.asciz "*"
string4: 	.asciz "="

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $1, %rax
	movq %rax, -8(%rbp)
while0:
	movq -8(%rbp), %rax
	pushq %rax
	movq $1000, %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile0
	leaq strout(%rip), %rdi
	leaq string1(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq -8(%rbp), %rax
	pushq %rax
	movq $2, %rax
	popq %r10
	imulq %r10, %rax
	movq %rax, -8(%rbp)
	jmp while0
endwhile0:
	leaq strout(%rip), %rdi
	leaq string2(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $1, %rax
	movq %rax, -8(%rbp)
while1:
	movq -8(%rbp), %rax
	pushq %rax
	movq $6, %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile1
	movq $1, %rax
	movq %rax, -16(%rbp)
while2:
	movq -16(%rbp), %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile2
	movq -16(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq strout(%rip), %rdi
	leaq string3(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq strout(%rip), %rdi
	leaq string4(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	imulq %r10, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq -16(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -16(%rbp)
	jmp while2
endwhile2:
	movq -8(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -8(%rbp)
	jmp while1
endwhile1:
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

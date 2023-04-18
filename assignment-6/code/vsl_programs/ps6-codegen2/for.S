.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "=========="

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	pushq $0
	pushq $0
	movq $0, %rax
	movq %rax, -8(%rbp)
	movq $6, %rax
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
	pushq %rax
	movq $0, %rax
	popq %r10
	cmpq %rax, %r10
	je else0
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif0
else0:
endif0:
	movq -8(%rbp), %rax
	pushq %rax
	movq $5, %rax
	popq %r10
	subq %r10, %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	popq %rdi
	popq %rsi
	call .min
	movq %rax, -24(%rbp)
	movq -8(%rbp), %rax
	pushq %rax
	movq $5, %rax
	popq %r10
	subq %r10, %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	popq %rdi
	popq %rsi
	call .max
	movq %rax, -32(%rbp)
while1:
	movq -24(%rbp), %rax
	pushq %rax
	movq -32(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile1
	movq -24(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq -24(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -24(%rbp)
	jmp while1
endwhile1:
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

.max:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
if1:
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jle else1
	movq -8(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	jmp endif1
else1:
	movq -16(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
endif1:
	movq $0, %rax
	movq %rbp, %rsp
	popq %rbp
	ret

.min:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
if2:
	movq -8(%rbp), %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jge else2
	movq -8(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	jmp endif2
else2:
	movq -16(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
endif2:
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

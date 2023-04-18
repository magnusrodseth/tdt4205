.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "Fibonnacci is now calculated, now using array as a linked list!"
string1: 	.asciz "at array["
string2: 	.asciz "] we find"
string3: 	.asciz "array[array["
string4: 	.asciz "]] is "

.section .bss
.align 8
.array: 	.zero 800

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $0
	pushq $0
	pushq $0
	movq $1, %rax
	pushq %rax
	movq $0, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq $2, %rax
	pushq %rax
	movq $1, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq $2, %rax
	movq %rax, -16(%rbp)
	movq $100, %rax
	movq %rax, -24(%rbp)
while0:
	movq -16(%rbp), %rax
	pushq %rax
	movq -24(%rbp), %rax
	popq %r10
	cmpq %rax, %r10
	jge endwhile0
	movq $1, %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	subq %r10, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	pushq %rax
	movq $2, %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %r10
	subq %r10, %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	popq %r10
	addq %r10, %rax
	pushq %rax
	movq -16(%rbp), %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	popq %rax
	movq %rax, (%r10)
	movq -16(%rbp), %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	addq %r10, %rax
	movq %rax, -16(%rbp)
	jmp while0
endwhile0:
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $0, %rax
	movq %rax, -8(%rbp)
while1:
	movq $1, %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	cmpq %rax, %r10
	jne endwhile1
	leaq strout(%rip), %rdi
	leaq string1(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq strout(%rip), %rdi
	leaq string2(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
if0:
	movq -8(%rbp), %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	pushq %rax
	movq $100, %rax
	popq %r10
	cmpq %rax, %r10
	jge else0
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
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq -8(%rbp), %rax
	leaq .array(%rip), %r10
	leaq (%r10, %rax, 8), %r10
	movq (%r10), %rax
	movq %rax, -8(%rbp)
	jmp endif0
else0:
	jmp endwhile1
endif0:
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

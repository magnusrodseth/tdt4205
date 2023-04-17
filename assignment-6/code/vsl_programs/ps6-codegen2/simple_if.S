.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "You"
string1: 	.asciz "Are"
string2: 	.asciz "WRONG1"
string3: 	.asciz "WRONG2"
string4: 	.asciz "Getting"
string5: 	.asciz "WRONG3"
string6: 	.asciz "Correct"
string7: 	.asciz "WRONG4"
string8: 	.asciz "Output"

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
if0:
	movq $1, %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	cmpq %rax, %r10
	jne else0
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
if1:
	movq $1, %rax
	pushq %rax
	movq $2, %rax
	popq %r10
	cmpq %rax, %r10
	jge else1
	leaq strout(%rip), %rdi
	leaq string1(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif1
else1:
	leaq strout(%rip), %rdi
	leaq string2(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
endif1:
if2:
	movq $-2, %rax
	pushq %rax
	movq $1, %rax
	popq %r10
	cmpq %rax, %r10
	jle else2
	leaq strout(%rip), %rdi
	leaq string3(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif2
else2:
if3:
	movq $1, %rax
	pushq %rax
	movq $2, %rax
	popq %r10
	cmpq %rax, %r10
	je else3
	leaq strout(%rip), %rdi
	leaq string4(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif3
else3:
	leaq strout(%rip), %rdi
	leaq string5(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
endif3:
endif2:
	leaq strout(%rip), %rdi
	leaq string6(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif0
else0:
	leaq strout(%rip), %rdi
	leaq string7(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
endif0:
	leaq strout(%rip), %rdi
	leaq string8(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
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

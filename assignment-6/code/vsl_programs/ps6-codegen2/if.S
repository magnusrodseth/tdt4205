.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: 	.asciz "a is less than 10"
string1: 	.asciz "a is more than -5"
string2: 	.asciz "a is less than or equal to -5"
string3: 	.asciz "a is equal to 5"
string4: 	.asciz "a is not equal to 0"
string5: 	.asciz "a is however 6"
string6: 	.asciz "a is however 7"
string7: 	.asciz "a is however 8"
string8: 	.asciz "a is neither 0, 6, 7 or 8, but"

.section .bss
.align 8

.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
if0:
	movq -8(%rbp), %rax
	pushq %rax
	movq $10, %rax
	popq %r10
	cmpq %rax, %r10
	jge else0
	leaq strout(%rip), %rdi
	leaq string0(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif0
else0:
endif0:
if1:
	movq -8(%rbp), %rax
	pushq %rax
	movq $-5, %rax
	popq %r10
	cmpq %rax, %r10
	jle else1
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
	movq -8(%rbp), %rax
	pushq %rax
	movq $5, %rax
	popq %r10
	cmpq %rax, %r10
	jne else2
	leaq strout(%rip), %rdi
	leaq string3(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif2
else2:
endif2:
if3:
	movq -8(%rbp), %rax
	pushq %rax
	movq $0, %rax
	popq %r10
	cmpq %rax, %r10
	je else3
	leaq strout(%rip), %rdi
	leaq string4(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
if4:
	movq -8(%rbp), %rax
	pushq %rax
	movq $6, %rax
	popq %r10
	cmpq %rax, %r10
	jne else4
	leaq strout(%rip), %rdi
	leaq string5(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif4
else4:
if5:
	movq -8(%rbp), %rax
	pushq %rax
	movq $7, %rax
	popq %r10
	cmpq %rax, %r10
	jne else5
	leaq strout(%rip), %rdi
	leaq string6(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif5
else5:
if6:
	movq -8(%rbp), %rax
	pushq %rax
	movq $8, %rax
	popq %r10
	cmpq %rax, %r10
	jne else6
	leaq strout(%rip), %rdi
	leaq string7(%rip), %rsi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	jmp endif6
else6:
	leaq strout(%rip), %rdi
	leaq string8(%rip), %rsi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
endif6:
endif5:
endif4:
	jmp endif3
else3:
endif3:
	movq $0, %rax
	movq %rbp, %rsp
	popq %rbp
	ret

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

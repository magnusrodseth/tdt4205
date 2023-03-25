.globl main

.section .rodata
intro: .string "On day %ld of Christmas my true love sent to me\n"
and_s: .string "and "

str01: .string "%ld partridge in a pear tree\n"
str02: .string "%ld turtle doves\n"
str03: .string "%ld french hens\n"
str04: .string "%ld calling birds\n"
str05: .string "%ld gold rings\n"
str06: .string "%ld geese a-laying\n"
str07: .string "%ld swans a-swimming\n"
str08: .string "%ld maids a-milking\n"
str09: .string "%ld ladies dancing\n"
str10: .string "%ld lords a-leaping\n"
str11: .string "%ld pipers piping\n"
str12: .string "%ld drummers drumming\n"

lines:
    .quad str01, str02, str03, str04, str05, str06
    .quad str07, str08, str09, str10, str11, str12

.section .text
main:
    pushq %rbp
    movq %rsp, %rbp

# Outer for loop
    movq $1, %r13
for_begin:
    cmpq $13, %r13
    jge for_end

# Print intro
part_intro:
    movq %r13, %rsi
    movq $intro, %rdi
    call printf

# Inner for loop
    movq %r13, %r14
inner_for_begin:
    cmpq $1, %r14
    jle inner_for_end

    # Calculate index into lines: (r14 - 1) * 64 + lines
    movq %r14, %rdi
    subq $1, %rdi
    salq $3, %rdi
    addq $lines, %rdi
    movq (%rdi), %rdi
    movq %r14, %rsi
    call printf
    
# Inner for end
    subq $1, %r14
    jmp inner_for_begin
inner_for_end:

# Print the last line
    cmpq $1, %r13
    je part_1

    movq $and_s, %rdi
    call printf

part_1:
    movq $str01, %rdi
    movq $1, %rsi
    call printf

    # print newline
    movq $10, %rdi
    call putchar

# Outer for end
    addq $1, %r13
    jmp for_begin
for_end:

    leave
    ret

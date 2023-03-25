.globl main

# Read-only data
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

# Text section
.section .text
main:
    pushq %rbp
    movq %rsp, %rbp

    movq $1, %r13 # r13 = i

# Iterate over days, from day 1 to day 12
for_i:
    cmpq $13, %r13 # i <= 12
    jge end_for_i # if i > 12, end

# Print the intro
print_intro:
    movq %r13, %rsi 
    movq $intro, %rdi 
    call printf

    movq %r13, %r14 # r14 = j

# Iterate over the lines, from the current day to day 1
for_j:
    cmpq $1, %r14 # j >= 1
    jle end_for_j # if j < 1, end

    # Calculate index into lines: (r14 - 1) * 64 + lines
    movq %r14, %rdi
    subq $1, %rdi # rdi = r14 - 1
    salq $3, %rdi # rdi = (r14 - 1) * 8
    addq $lines, %rdi 
    movq (%rdi), %rdi
    movq %r14, %rsi # Move j into rsi
    call printf

    subq $1, %r14 # j--
    jmp for_j
    
# End of for_j
end_for_j:
    # Print the last line
    cmpq $1, %r13 # i == 1
    je print_first_line # if i == 1, print the first line

    movq $and_s, %rdi 
    call printf

print_first_line:
    # Print the first line
    movq $str01, %rdi 
    movq $1, %rsi 
    call printf

    # Print newline
    movq $10, %rdi
    call putchar

    addq $1, %r13 # i++
    jmp for_i 

# End of for_i
end_for_i:
    leave
    ret

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

    /*
        TODO: Task 1 - Put your code here.

        Your task: 
        Implement a GNU Assembly program that prints a alightly modified version of the "Twelve Days of Christmas" song.
        Instead of "first day, second day" or "1st day, 2nd day" etc, your program may output "On day N of Christmas my true love gave to me".
        This song is cumulative - each day will include the gifts of every previous day.
        (i.e. day 1 will include only a partridge, while day 2 will include two turtle doves AND a partridge).
        Use printf to output the lyrics of the song to terminal.
        A suggestion for the first lines of the output is added below.


        HINTS:
        - The registers %rdi and %rsi are used for the first two arguments in a function call (like printf).
        - Some registers are overwritten by functions (including printf!) This includes %rax, %rbx, %rcx, %rdx, %rdi, %rsi, %rbp, %rsp, and %r8-r15.
        - The order of comparisons are (for some reason) reversed! This means that cmp %r8, %r9 will compare r9 vs r8.
        - Create a debug helper string that you can call with printf for slightly easier debugging.
        - Consult the examples in (../examples/) for inspiration. Make them with `make`.
        - Use a cheat sheet like https://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html or https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf
        - Compile this with `make` once finished.


        BONUS POINTS (Can only be exchanged for bragging rights)
        - Use putchar to easily create new lines so that the finished song doesn't become a giant wall of text.
        - Make the song include "and" before the last line - i.e., "AND 1 partridge in a pear tree" - in every verse except the first.


        SUGGESTED OUTPUT:
        On day 1 of Christmas my true love sent to me
        1 partridge in a pear tree

        On day 2 of Christmas my true love sent to me
        2 turtle doves
        and 1 partridge in a pear tree

        On day 3 of Christmas my true love sent to me
        3 french hens
        2 turtle doves
        and 1 partridge in a pear tree

        ...
    */

    leave
    ret

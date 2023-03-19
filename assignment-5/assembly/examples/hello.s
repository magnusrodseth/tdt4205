.globl main

.section .data
hello:
    .string "Hello World! %ld\n"

.section .text
main:                       # Function main:
    pushq %rbp              # Push frame pointer to stack
    movq %rsp, %rbp         # Save stack pointer as frame pointer

    movq $42, %rsi          # Setting arg 2 to constant value 42
    movq $hello, %rdi       # Setting arg 1 to defined string "Hello World! %ld\n"
    call printf             # Print using the arguments in %rsi and %rdi

    leave                   # Done with main: reverting pointers and pop from stack 
    ret                     # Return from main

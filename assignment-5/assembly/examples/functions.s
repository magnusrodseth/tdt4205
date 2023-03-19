.globl main

.section .rodata
hello:
    .string "Hello World!\n"
current_value_string:
    .string "Current value is %ld\n"
counting_string:
    .string "Value is %ld\n"

.section .text
main:                       # Function main:
    push %rbp              # Push frame pointer to stack
    movq %rsp, %rbp         # Save stack pointer as frame pointer

    call func_say_hello     # Calling function `func_say_hello`

    movq $3, %rdi
    call func_count         # Calling function `func_count` with parameter max=3

    movq $10, %rdi
    call func_count         # Calling function `func_count` with parameter max=10

    leave                   # Done: reverting pointers and pop from stack 
    ret                     # Returning

func_say_hello:
    push %rbp              # Push frame pointer to stack
    movq %rsp, %rbp         # Save stack pointer as frame pointer

    movq $hello, %rdi
    movq $0, %rsi
    call printf

    leave                   # Done: reverting pointers and pop from stack 
    ret                     # Returning

/*
    Count from 0 to provided argument (in %rdi)

    Args:
        (%rdi) max (quad) - Upper limit of counting
*/
func_count:
    push %rbp
    movq %rsp, %rbp

    movq $0, %r12           # r12 = Incrementing counter; 0
    movq %rdi, %r13         # r13 = Max value; argument 1

    # Loop until r8 > r9
    func_count_loop:

    inc %r12               # r12++
    cmp %r12, %r13         # Check r12 vs r13
    jl done                 # ..and jump if r13 < r12

    # If not done, print string
    # Prepare arguments
    movq $current_value_string, %rdi
    movq %r12, %rsi

    # Print
    call printf

    jmp func_count_loop     # Loop

    done:
    leave
    ret




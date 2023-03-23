.globl main

.section .data
hello:
    .string "Hello World! %ld\n"

.section .text
main:                       
    pushq %rbp              
    movq %rsp, %rbp         

    movq $42, %rsi          
    movq $hello, %rdi       
    call printf             

    leave                   
    ret                     

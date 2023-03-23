.globl main
.section .data

number_of_guesses: .quad 5
guesses: .quad 1, 100, 13, 50, 42
guess_correct: .string "Guessed number %ld is correct!!\n"
guess_too_low: .string "Guessed number %ld is too low.\n"
guess_too_high: .string "Guessed number %ld is too high.\n"


line1: .string "I am by birth a Genevese; and my family is one of the most distinguished of that republic.\n"
line2: .string "My ancestors had been for many years counsellors and syndics; and my father had filled several public situations with honour and reputation.\n"
line3: .string "He was respected by all who knew him for his integrity and indefatigable attention to public business.\n"
line4: .string "He passed his younger days perpetually occupied by the affairs of his country; and it was not until the decline of life that he thought of marrying, and bestowing on the state sons who might carry his virtues and his name down to posterity.\n"
lines: .quad 4
book: .quad line1, line2, line3, line4

debug: .string "Value is %ld\n"

.section .text
main:
    pushq %rbp
    movq %rsp, %rbp

    call guess_number

    movq $'\n', %rdi
    call putchar

    call read_book

    leave
    ret

guess_number:
    pushq %rbp
    movq %rsp, %rbp

    movq $42, %r12                      # Secret number (r12)
    movq $0, %r13                       # i=0 (r13)

    guess_number_loop:
    movq $number_of_guesses, %rdi       # Load ADDRESS to %rdi
    movq (%rdi), %rdi                   # Dereference %rdi
    cmp %rdi, %r13                      # r13 vs rdi
    jge guess_number_loop_end           # Jump if i >= number_of_guesses
    
    movq $guesses, %rdi                 # Load "guesses" address
    movq (%rdi, %r13, 8), %rdi          # Dereference guess i; guess = guesses[i]
    movq %r12, %rsi                     # Move secret to arg 2 position
    call compare_number                 # Call subfunction

    inc %r13                            # i++
    jmp guess_number_loop
    guess_number_loop_end:

    leave
    ret

/*
    Compare guess in %rdi to secret in %rsi

    Args:
        (%rdi) guess (quad) - One user guess
        (%rsi) secret (quad) - Secret number to guess
*/
compare_number:
    pushq %rbp
    movq %rsp, %rbp

    cmp %rsi, %rdi                      # rdi vs rsi
    movq %rdi, %rsi                     # Preemptively move guess to %rdi
    jg greater                          # Jump to "greater" if guess > secret
    jl lesser                           # Jump to "lesser" if guess < secret
                                        # Otherwise, guess == secret

    # Equal
    movq $guess_correct, %rdi
    jmp compare_number_end

    # Greater
    greater:
    movq $guess_too_high, %rdi
    jmp compare_number_end

    # Lesser
    lesser:
    movq $guess_too_low, %rdi

    compare_number_end:

    call printf                         # Print message and guess

    leave
    ret

read_book:
    pushq %rbp
    movq %rsp, %rbp

    movq $0, %r12
    read_book_loop:
    movq $lines, %rdi                   # Reading lines address
    movq (%rdi), %rdi                   # Dereferencing lines
    cmp %rdi, %r12                      # r12 vs rdi
    jge read_book_loop_end              # Jump if i >= max

    movq $book, %rdi                    # Reading address of book pointer
    movq (%rdi, %r12, 8), %rdi          # Read nth element; e = lines[i]
    movq $0, %rsi                       # No effect, just for pushing even number of arguments (won't matter here)
    call printf                         # Printing line i; Note printf takes string address so no need for dereferencing

    inc %r12                            # i ++
    jmp read_book_loop
    read_book_loop_end:
    leave
    ret

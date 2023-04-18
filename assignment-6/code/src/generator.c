#include <vslc.h>

// This header defines a bunch of macros we can use to emit assembly to stdout
#include "emit.h"

// In the System V calling convention, the first 6 integer parameters are passed in registers
#define NUM_REGISTER_PARAMS 6
static const char *REGISTER_PARAMS[6] = {RDI, RSI, RDX, RCX, R8, R9};

// Takes in a symbol of type SYMBOL_FUNCTION, and returns how many parameters the function takes
#define FUNC_PARAM_COUNT(func) ((func)->node->children[1]->n_children)

static void generate_string_table(void);
static void generate_global_variables(void);
static void generate_function(symbol_t *function);
static void generate_expression(node_t *expression);
static void generate_statement(node_t *node);
static void generate_main(symbol_t *first);
static void generate_block_statement(node_t *node);
static symbol_t *get_topmost_function();
static bool is_less_than_relation(const char *str);
static bool is_greater_than_relation(const char *str);
static bool is_equal_relation(const char *str);
static bool is_not_equal_relation(const char *str);

/* Global variable used to make the functon currently being generated acessiable from anywhere */
static symbol_t *current_function;

static const int BUFFER_SIZE_IN_BYTES = 1024;

/**
 * Global variable used to keep track of the innermost while loop, so we can jump to the correct
 * place when a break statement is encountered 
 **/
static int while_counter = 0;

/**
 * Global variable used to keep track of the current if statement.
 */
static int if_counter = 0;

static bool is_less_than_relation(const char *str) {
    return strcmp(str, "<") == 0;
}

static bool is_greater_than_relation(const char *str) {
    return strcmp(str, ">") == 0;
}

static bool is_equal_relation(const char *str) {
    return strcmp(str, "=") == 0;
}

static bool is_not_equal_relation(const char *str) {
    return strcmp(str, "!=") == 0;
}

static symbol_t *get_topmost_function() {
    symbol_t *first_function = NULL;
    for (size_t i = 0; i < global_symbols->n_symbols; i++) {
        symbol_t *symbol = global_symbols->symbols[i];
        if (symbol->type != SYMBOL_FUNCTION)
            continue;
        if (!first_function)
            first_function = symbol;
        generate_function(symbol);
    }

    if (first_function == NULL) {
        fprintf(stderr, "error: program contained no functions\n");
        exit(EXIT_FAILURE);
    }

    return first_function;
}

/* Entry point for code generation */
void generate_program(void) {
    generate_string_table();
    generate_global_variables();

    DIRECTIVE(".text");
    symbol_t *first_function = get_topmost_function();
    generate_main(first_function);
}

/* Prints one .asciz entry for each string in the global string_list */
static void generate_string_table(void) {
    DIRECTIVE(".section %s", ASM_STRING_SECTION);
    // These strings are used by printf
    DIRECTIVE("intout: .asciz \"%s\"", "%ld ");
    DIRECTIVE("strout: .asciz \"%s\"", "%s ");
    // This string is used by the entry point-wrapper
    DIRECTIVE("errout: .asciz \"%s\"", "Wrong number of arguments");

    for (size_t i = 0; i < string_list_len; i++) {
        DIRECTIVE("string%ld: \t.asciz %s", i, string_list[i]);
    }

    DIRECTIVE();
}

/* Prints .zero entries in the .bss section to allocate room for global variables and arrays */
static void generate_global_variables(void) {
    DIRECTIVE(".section %s", ASM_BSS_SECTION);
    DIRECTIVE(".align 8");

    for (size_t i = 0; i < global_symbols->n_symbols; i++) {
        symbol_t *symbol = global_symbols->symbols[i];
        if (symbol->type == SYMBOL_GLOBAL_VAR) {
            DIRECTIVE(".%s: \t.zero 8", symbol->name);
        } else if (symbol->type == SYMBOL_GLOBAL_ARRAY) {
            node_t *child = symbol->node->children[1];
            if (child->type != NUMBER_DATA) {
                fprintf(stderr, "error: length of array '%s' is not compile time known", symbol->name);
                exit(EXIT_FAILURE);
            }
            int64_t length = *(int64_t *)child->data;
            DIRECTIVE(".%s: \t.zero %ld", symbol->name, length * 8);
        }
    }

    DIRECTIVE();
}

/* Prints the entry point. preamble, statements and epilouge of the given function */
static void generate_function(symbol_t *function) {
    LABEL(".%s", function->name);
    current_function = function;

    PUSHQ(RBP);
    MOVQ(RSP, RBP);

    // Up to 6 prameters have been passed in registers. Place them on the stack instead
    for (size_t i = 0; i < FUNC_PARAM_COUNT(function) && i < NUM_REGISTER_PARAMS; i++) {
        PUSHQ(REGISTER_PARAMS[i]);
    }

    // Now, for each local variable, push 8-byte 0 values to the stack
    for (size_t i = 0; i < function->function_symtable->n_symbols; i++) {
        symbol_t *symbol = function->function_symtable->symbols[i];
        if (symbol->type == SYMBOL_LOCAL_VAR) {
            PUSHQ("$0");
        }
    }

    node_t *function_body = function->node->children[2];
    generate_statement(function_body);

    // In case the function didn't return, return 0 here
    MOVQ("$0", RAX);

    // leaveq is written out manually, to increase clarity of what happens
    MOVQ(RBP, RSP);
    POPQ(RBP);
    RET;

    DIRECTIVE();
}

static void generate_function_call(node_t *call) {
    symbol_t *symbol = call->children[0]->symbol;
    if (symbol->type != SYMBOL_FUNCTION) {
        fprintf(stderr, "error: '%s' is not a function\n", symbol->name);
        exit(EXIT_FAILURE);
    }

    node_t *argument_list = call->children[1];

    int parameter_count = FUNC_PARAM_COUNT(symbol);
    if (parameter_count != argument_list->n_children) {
        fprintf(stderr, "error: function '%s' expects '%d' arguments, but '%ld' were given\n",
                symbol->name, parameter_count, argument_list->n_children);
        exit(EXIT_FAILURE);
    }

    // We evaluate all parameters from right to left, pushing them to the stack
    for (int i = parameter_count - 1; i >= 0; i--) {
        generate_expression(argument_list->children[i]);
        PUSHQ(RAX);
    }

    // Up to 6 parameters should be passed through registers instead. Pop them off the stack
    for (size_t i = 0; i < parameter_count && i < NUM_REGISTER_PARAMS; i++) {
        POPQ(REGISTER_PARAMS[i]);
    }

    EMIT("call .%s", symbol->name);

    // Now pop away any stack passed parameters still left on the stack, by moving %rsp upwards
    if (parameter_count > NUM_REGISTER_PARAMS) {
        EMIT("addq $%d, %s", (parameter_count - NUM_REGISTER_PARAMS) * 8, RSP);
    }
}

/* Returns a string for accessing the quadword referenced by node */
static const char *generate_variable_access(node_t *node) {
    assert(node->type == IDENTIFIER_DATA);

    static char result[100];

    symbol_t *symbol = node->symbol;
    switch (symbol->type) {
        case SYMBOL_GLOBAL_VAR: {
            snprintf(result, sizeof(result), ".%s(%s)", symbol->name, RIP);
            return result;
        }
        case SYMBOL_LOCAL_VAR: {
            // If we have more than 6 parameters, subtract away the hole in the sequence numbers
            int call_frame_offset = symbol->sequence_number;
            if (FUNC_PARAM_COUNT(current_function) > NUM_REGISTER_PARAMS) {
                call_frame_offset -= FUNC_PARAM_COUNT(current_function) - NUM_REGISTER_PARAMS;
            }

            // The stack grows down, in multiples of 8, and sequence number 0 corresponds to -8
            call_frame_offset = (-call_frame_offset - 1) * 8;

            snprintf(result, sizeof(result), "%d(%s)", call_frame_offset, RBP);
            return result;
        }
        case SYMBOL_PARAMETER: {
            int call_frame_offset;
            // Handle the first 6 parameters differently
            if (symbol->sequence_number < NUM_REGISTER_PARAMS) {
                // Move along down the stack, with parameter 0 at position -8(%rbp)
                call_frame_offset = (-symbol->sequence_number - 1) * 8;
            } else {
                // Parameter 6 is at 16(%rbp), with further parameters moving up from there
                call_frame_offset = 16 + (symbol->sequence_number - NUM_REGISTER_PARAMS) * 8;
            }

            snprintf(result, sizeof(result), "%d(%s)", call_frame_offset, RBP);
            return result;
        }
        case SYMBOL_FUNCTION: {
            fprintf(stderr, "error: symbol '%s' is a function, not a variable\n", symbol->name);
            exit(EXIT_FAILURE);
        }
        case SYMBOL_GLOBAL_ARRAY: {
            fprintf(stderr, "error: symbol '%s' is an array, not a variable\n", symbol->name);
            exit(EXIT_FAILURE);
        }
        default:
            assert(false && "Unknown variable symbol type");
    }
}

/**
 * Returns a string for accessing the quadword referenced by the ARRAY_INDEXING node.
 * Code for evaluating the index will be emitted, which can potentially mess with all registers.
 * The resulting memory access string will not make use of the %rax register.
 */
static const char *generate_array_access(node_t *node) {
    assert(node->type == ARRAY_INDEXING);

    symbol_t *symbol = node->children[0]->symbol;
    if (symbol->type != SYMBOL_GLOBAL_ARRAY) {
        fprintf(stderr, "error: symbol '%s' is not an array\n", symbol->name);
        exit(EXIT_FAILURE);
    }

    // Calculate the index of the array into %rax
    node_t *index = node->children[1];
    generate_expression(index);

    // Place the base of the array into %r10
    EMIT("leaq .%s(%s), %s", symbol->name, RIP, R10);

    // Place the exact position of the element we wish to access, into %r10
    EMIT("leaq (%s, %s, 8), %s", R10, RAX, R10);

    // Now, the element we wish to access is stored at %r10 exactly, so just use that to reference it
    return MEM(R10);
}

/* Generates code to evaluate the expression, and place the result in %rax */
static void generate_expression(node_t *expression) {
    switch (expression->type) {
        case NUMBER_DATA: {
            // Simply place the number into RAX
            EMIT("movq $%ld, %s", *(int64_t *)expression->data, RAX);
            break;
        }
        case IDENTIFIER_DATA: {
            // Load the variable, and put the result in RAX
            MOVQ(generate_variable_access(expression), RAX);
            break;
        }
        case ARRAY_INDEXING: {
            // Load the value pointed to by array[idx], and put the result in RAX
            MOVQ(generate_array_access(expression), RAX);
            break;
        }
        case EXPRESSION: {
            char *data = expression->data;
            node_t *left = expression->children[0];
            node_t *right = expression->children[1];

            bool is_function_call = strcmp(data, "call") == 0;
            bool is_plus = strcmp(data, "+") == 0;
            bool is_minus = strcmp(data, "-") == 0;
            bool is_multiplication = strcmp(data, "*") == 0;
            bool is_division = strcmp(data, "/") == 0;

            if (is_function_call) {
                generate_function_call(expression);
            } else if (is_plus) {
                generate_expression(left);
                PUSHQ(RAX);
                generate_expression(right);
                POPQ(R10);
                ADDQ(R10, RAX);
            } else if (is_minus) {
                if (expression->n_children == 1) {
                    // Unary minus
                    generate_expression(left);
                    NEGQ(RAX);
                } else {
                    // Binary minus. Evaluate RHS first, to get the result in RAX easier
                    generate_expression(right);
                    PUSHQ(RAX);
                    generate_expression(left);
                    POPQ(R10);
                    SUBQ(R10, RAX);
                }
            } else if (is_multiplication) {
                // Multiplication does not need to do sign extend
                generate_expression(left);
                PUSHQ(RAX);
                generate_expression(right);
                POPQ(R10);
                IMULQ(R10, RAX);
            } else if (is_division) {
                generate_expression(right);
                PUSHQ(RAX);
                generate_expression(left);
                CQO;  // Sign extend RAX -> RDX:RAX
                POPQ(R10);
                IDIVQ(R10);  // Didivde RDX:RAX by R10, placing the result in RAX
            } else {
                assert(false && "Unknown expression operation");
            }
            break;
        }
        default:
            assert(false && "Unknown expression type");
    }
}

static void generate_assignment_statement(node_t *statement) {
    node_t *destination = statement->children[0];
    node_t *expression = statement->children[1];
    generate_expression(expression);

    if (destination->type == IDENTIFIER_DATA)
        MOVQ(RAX, generate_variable_access(destination));
    else {
        // Store rax until the final address of the array element is found,
        // since array index calculation can change registers
        PUSHQ(RAX);
        const char *destination_memory = generate_array_access(destination);
        POPQ(RAX);
        MOVQ(RAX, destination_memory);
    }
}

static void generate_print_statement(node_t *statement) {
    for (size_t i = 0; i < statement->n_children; i++) {
        node_t *child = statement->children[i];
        if (child->type == STRING_DATA) {
            EMIT("leaq strout(%s), %s", RIP, RDI);
            EMIT("leaq string%ld(%s), %s", *(uint64_t *)child->data, RIP, RSI);
        } else {
            generate_expression(child);
            MOVQ(RAX, RSI);
            EMIT("leaq intout(%s), %s", RIP, RDI);
        }
        EMIT("call safe_printf");
    }

    MOVQ("$'\\n'", RDI);
    EMIT("call putchar");
}

static void generate_return_statement(node_t *statement) {
    node_t *expression = statement->children[0];
    generate_expression(expression);
    MOVQ(RBP, RSP);
    POPQ(RBP);
    RET;
}

static void generate_relation(node_t *relation) {
    assert(relation->n_children == 2);

    node_t *left = relation->children[0];
    node_t *right = relation->children[1];

    generate_expression(left);
    // Push left onto the stack
    PUSHQ(RAX);
    // Evaluate right and move it into RAX
    generate_expression(right);
    // Pop left into R10
    POPQ(R10);
    // Compare left and right
    CMPQ(RAX, R10);
}

static void generate_if_statement(node_t *statement) {
    int local_counter = if_counter;
    if_counter++;

    LABEL("if%d", local_counter);

    assert(statement->n_children == 2 || statement->n_children == 3);
    node_t *relation = statement->children[0];
    node_t *then_statement = statement->children[1];

    generate_relation(relation);

    char *data = relation->data;

    char else_label[BUFFER_SIZE_IN_BYTES];
    memset(else_label, 0, BUFFER_SIZE_IN_BYTES);
    snprintf(else_label, BUFFER_SIZE_IN_BYTES, "else%d", local_counter);

    if (is_equal_relation(data)) {
        JNE(else_label);
    } else if (is_not_equal_relation(data)) {
        JE(else_label);
    } else if (is_less_than_relation(data)) {
        JGE(else_label);
    } else if (is_greater_than_relation(data)) {
        JLE(else_label);
    } else {
        assert(false && "Unknown relation");
    }

    generate_statement(then_statement);

    // Jump to end of if statement
    char endif_label[BUFFER_SIZE_IN_BYTES];
    memset(endif_label, 0, BUFFER_SIZE_IN_BYTES);
    snprintf(endif_label, BUFFER_SIZE_IN_BYTES, "endif%d", local_counter);
    JMP(endif_label);

    LABEL("else%d", local_counter);

    bool has_else = statement->n_children == 3;
    if (has_else) {
        node_t *else_statement = statement->children[2];
        generate_statement(else_statement);
    }

    LABEL("endif%d", local_counter);
}

static void generate_while_statement(node_t *statement) {
    int local_counter = while_counter;
    while_counter++;

    LABEL("while%d", local_counter);

    assert(statement->n_children == 2);
    node_t *relation = statement->children[0];
    node_t *block = statement->children[1];

    generate_relation(relation);

    char *data = relation->data;

    char end_label[BUFFER_SIZE_IN_BYTES];
    memset(end_label, 0, BUFFER_SIZE_IN_BYTES);
    snprintf(end_label, BUFFER_SIZE_IN_BYTES, "endwhile%d", local_counter);

    if (is_equal_relation(data)) {
        JNE(end_label);
    } else if (is_not_equal_relation(data)) {
        JE(end_label);
    } else if (is_less_than_relation(data)) {
        JGE(end_label);
    } else if (is_greater_than_relation(data)) {
        JLE(end_label);
    } else {
        assert(false && "Unknown relation");
    }

    generate_block_statement(block);

    // jump back to the beginning of the while loop
    char while_label[BUFFER_SIZE_IN_BYTES];
    memset(while_label, 0, BUFFER_SIZE_IN_BYTES);
    snprintf(while_label, BUFFER_SIZE_IN_BYTES, "while%d", local_counter);
    JMP(while_label);

    // End of while loop, and continuation of program flow
    LABEL("endwhile%d", local_counter);
}

static void generate_break_statement() {
    // When hitting a break, we can merely decrement the innermost while counter, and
    // jump to the label with the corresponding number of the current value of `while_counter`.
    while_counter--;

    char end_label[BUFFER_SIZE_IN_BYTES];
    memset(end_label, 0, BUFFER_SIZE_IN_BYTES);
    snprintf(end_label, BUFFER_SIZE_IN_BYTES, "endwhile%d", while_counter);
    JMP(end_label);
}

static void generate_block_statement(node_t *node) {
    // All handling of pushing and popping scores has already been done
    // Just generate the statements that make up the statement body, one by one
    node_t *statement_list = node->children[node->n_children - 1];
    for (size_t i = 0; i < statement_list->n_children; i++)
        generate_statement(statement_list->children[i]);
}

/* Recursively generate the given statement node, and all sub-statements. */
static void generate_statement(node_t *node) {
    switch (node->type) {
        case BLOCK:
            generate_block_statement(node);
            break;
        case ASSIGNMENT_STATEMENT:
            generate_assignment_statement(node);
            break;
        case PRINT_STATEMENT:
            generate_print_statement(node);
            break;
        case RETURN_STATEMENT:
            generate_return_statement(node);
            break;
        case IF_STATEMENT:
            generate_if_statement(node);
            break;
        case WHILE_STATEMENT:
            generate_while_statement(node);
            break;
        case BREAK_STATEMENT:
            generate_break_statement();
            break;
        default:
            assert(false && "Unknown statement type");
    }
}

static void generate_safe_printf(void) {
    LABEL("safe_printf");

    PUSHQ(RBP);
    MOVQ(RSP, RBP);
    // This is a bitmask that abuses how negative numbers work, to clear the last 4 bits
    // A stack pointer that is not 16-byte aligned, will be moved down to a 16-byte boundary
    ANDQ("$-16", RSP);
    EMIT("call printf");
    // Cleanup the stack back to how it was
    MOVQ(RBP, RSP);
    POPQ(RBP);
    RET;
}

static void generate_main(symbol_t *first) {
    // Make the globally available main function
    LABEL("main");

    // Save old base pointer, and set new base pointer
    PUSHQ(RBP);
    MOVQ(RSP, RBP);

    // Which registers argc and argv are passed in
    const char *argc = RDI;
    const char *argv = RSI;

    const size_t expected_args = FUNC_PARAM_COUNT(first);

    SUBQ("$1", argc);  // argc counts the name of the binary, so subtract that
    EMIT("cmpq $%ld, %s", expected_args, argc);
    JNE("ABORT");  // If the provdied number of arguments is not equal, go to the abort label

    if (expected_args == 0)
        goto skip_args;  // No need to parse argv

    // Now we emit a loop to parse all parameters, and push them to the stack,
    // in right-to-left order

    // First move the argv pointer to the vert rightmost parameter
    EMIT("addq $%ld, %s", expected_args * 8, argv);

    // We use rcx as a counter, starting at the number of arguments
    MOVQ(argc, RCX);
    LABEL("PARSE_ARGV");  // A loop to parse all parameters
    PUSHQ(argv);          // push registers to caller save them
    PUSHQ(RCX);

    // Now call strtol to parse the argument
    EMIT("movq (%s), %s", argv, RDI);  // 1st argument, the char *
    MOVQ("$0", RSI);                   // 2nd argument, a null pointer
    MOVQ("$10", RDX);                  // 3rd argument, we want base 10
    EMIT("call strtol");

    // Restore caller saved registers
    POPQ(RCX);
    POPQ(argv);
    PUSHQ(RAX);  // Store the parsed argument on the stack

    SUBQ("$8", argv);         // Point to the previous char*
    EMIT("loop PARSE_ARGV");  // Loop uses RCX as a counter automatically

    // Now, pop up to 6 arguments into registers instead of stack
    for (size_t i = 0; i < expected_args && i < NUM_REGISTER_PARAMS; i++)
        POPQ(REGISTER_PARAMS[i]);

skip_args:

    EMIT("call .%s", first->name);
    MOVQ(RAX, RDI);     // Move the return value of the function into RDI
    EMIT("call exit");  // Exit with the return value as exit code

    LABEL("ABORT");  // In case of incorrect number of arguments
    EMIT("leaq errout(%s), %s", RIP, RDI);
    EMIT("call puts");  // print the errout string
    MOVQ("$1", RDI);
    EMIT("call exit");  // Exit with return code 1

    generate_safe_printf();

    // Declares global symbols we use or emit, such as main, printf and putchar
    DIRECTIVE("%s", ASM_DECLARE_SYMBOLS);
}

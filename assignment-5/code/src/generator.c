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
static void generate_global_functions(void);
static void generate_function(symbol_t *function);
static void generate_statement(node_t *node);
static void generate_main(symbol_t *first);
static symbol_t *topmost_function();

static symbol_t *topmost_function() {
    symbol_t *topmost = NULL;
    for (int i = 0; i < global_symbols->n_symbols; i++) {
        symbol_t *symbol = global_symbols->symbols[i];
        if (symbol->type == SYMBOL_FUNCTION) {
            topmost = symbol;
            break;
        }
    }

    return topmost;
}

static void generate_global_functions(void) {
    DIRECTIVE(".section .text");
    // For each function in global_symbols, generate it using generate_function()
    for (int i = 0; i < global_symbols->n_symbols; i++) {
        symbol_t *symbol = global_symbols->symbols[i];
        if (symbol->type == SYMBOL_FUNCTION) {
            generate_function(symbol);
        }
    }
}

/* Entry point for code generation */
void generate_program(void) {
    generate_string_table();
    generate_global_variables();
    generate_global_functions();

    // In VSL, the topmost function in a program is its entry point.
    // We want to be able to take parameters from the command line,
    // and have them be sent into the entry point function.
    //
    // Due to the fact that parameters are all passed as strings,
    // and passed as the (argc, argv)-pair, we need to make a wrapper for our entry function.
    // This wrapper handles string -> int64_t conversion, and is already implemented.
    // call generate_main ( <entry point function symbol> );
    symbol_t *first = topmost_function();
    assert(first != NULL);
    generate_main(first);
}

/* Prints one .asciz entry for each string in the global string_list */
static void generate_string_table(void) {
    DIRECTIVE(".section .rodata");
    // These strings are used by printf
    DIRECTIVE("intout: .asciz \"%s\"", "%ld ");
    DIRECTIVE("strout: .asciz \"%s\"", "%s ");
    // This string is used by the entry point-wrapper
    DIRECTIVE("errout: .asciz \"%s\"", "Wrong number of arguments");

    // Print all strings in the program here, with labels you can refer to later
    for (int i = 0; i < string_list_len; i++) {
        char *string = string_list[i];
        DIRECTIVE("string%d: .asciz %s", i, string);
    }

    printf("\n");
}

/* Prints .zero entries in the .bss section to allocate room for global variables and arrays */
static void generate_global_variables(void) {
    DIRECTIVE(".section .bss");
    DIRECTIVE(".align 8");
    const int VARIABLE_SIZE_IN_BYTES = 8;

    for (int i = 0; i < global_symbols->n_symbols; i++) {
        symbol_t *symbol = global_symbols->symbols[i];

        if (symbol->type == SYMBOL_GLOBAL_VAR) {
            // Normal global variables take up 8 bytes, so we set aside 8 bytes using `.zero 8`
            DIRECTIVE(".%s: .zero %d", symbol->name, VARIABLE_SIZE_IN_BYTES);
        } else if (symbol->type == SYMBOL_GLOBAL_ARRAY) {
            // Arrays can take up multiple 8 byte values, so we need to compute their total size.
            // Array symbols store a pointer to their definition in the `node` field.
            node_t *array_node = symbol->node;
            assert(array_node->type == ARRAY_DECLARATION && array_node->n_children == 2);

            // The array_node’s second child should be a NUMBER_DATA node containing the length of the array.
            node_t *length_node = array_node->children[1];
            assert(length_node->type == NUMBER_DATA);
            int64_t length = *(int64_t *)length_node->data;

            // Use the `.zero` directive to set aside the correct amount of space.
            // Multiply the length by 8, to get the array size in bytes.
            DIRECTIVE(".%s: .zero %ld", symbol->name, length * VARIABLE_SIZE_IN_BYTES);
        }
    }

    printf("\n");
}

/* Prints the entry point. preable, statements and epilouge of the given function */
static void generate_function(symbol_t *function) {
    // Initialize the call frame and take parameters
    LABEL(".%s", function->name);

    // Save old base pointer, and set new base pointer
    PUSHQ(RBP);
    MOVQ(RSP, RBP);

    // Push registers with parameters onto the stack
    for (int i = 0; i < NUM_REGISTER_PARAMS; i++) {
        PUSHQ(REGISTER_PARAMS[i]);
    }

    // If the function takes more than 6 parameters, the ABI defines that the caller
    // should push the extra parameters to the stack, from right to left, before the function is called,
    // which means they end up above the return address on the stack.
    // TODO: Handle this case

    // Push local variables onto the stack
    for (int i = 0; i < function->function_symtable->n_symbols; i++) {
        symbol_t *symbol = function->function_symtable->symbols[i];
        if (symbol->type == SYMBOL_LOCAL_VAR) {
            PUSHQ("$0");
        }
    }

    // The function body can be sent to generate_statement()
    assert(function->node->n_children == 3);
    node_t *body = function->node->children[2];
    generate_statement(body);

    // Reset the %rsp to %rbp
    MOVQ(RBP, RSP);
    // Pop the caller’s %rbp value that we saved to the stack previously
    POPQ(RBP);

    printf("\n");
}

static void generate_function_call(node_t *call) {
    // TODO 2.4.3
}

static void generate_expression(node_t *expression) {
    // TODO: 2.4.1
}

static void generate_assignment_statement(node_t *statement) {
    // TODO: 2.4.2
}

static void generate_print_statement(node_t *statement) {
    // TODO: 2.4.4
}

static void generate_return_statement(node_t *statement) {
    // TODO: 2.4.5 Store the value in %rax and jump to the function epilogue
}

/* Recursively generate the given statement node, and all sub-statements. */
static void generate_statement(node_t *node) {
    node_t *child = node->children[0];
    switch (node->type) {
        case EXPRESSION:
            generate_expression(child);
            break;
        case ASSIGNMENT_STATEMENT:
            generate_assignment_statement(child);
            break;
        case PRINT_STATEMENT:
            generate_print_statement(child);
            break;
        case RETURN_STATEMENT:
            generate_return_statement(child);
            break;
        default:
            for (int i = 0; i < node->n_children; i++) {
                generate_statement(node->children[i]);
            }
            break;
    }
}

// Generates a wrapper, to be able to use a vsl function as our entrypoint
static void generate_main(symbol_t *first) {
    // Make the globally available main function
    DIRECTIVE(".globl main");
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
    MOVQ("$errout", RDI);
    EMIT("call puts");  // print the errout string
    MOVQ("$1", RDI);
    EMIT("call exit");  // Exit with return code 1
}

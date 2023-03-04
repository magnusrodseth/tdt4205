#include <vslc.h>

/* Global symbol table and string list */
symbol_table_t *global_symbols;
char **string_list;
size_t string_list_len;
size_t string_list_capacity;

static void find_globals(void);
static void bind_names(symbol_table_t *local_symbols, node_t *root);
static void print_symbol_table(symbol_table_t *table, int nesting);
static void destroy_symbol_tables(void);

static size_t add_string(char *string);
static void print_string_list(void);
static void destroy_string_list(void);

/* External interface */

/**
 * Creates a global symbol table, and local symbol tables for each function.
 * While building the symbol tables:
 *  - All usages of symbols are bound to their symbol table entries.
 *  - All strings are entered into the string_list
 */
void create_tables(void) {
    // TODO:
    //  First, use find_globals() to create the global symbol table.
    //  As global symbols are added, function symbols get their own local symbol tables as well.
    find_globals();

    //  Once all global symols are added, go through all functions bodies.
    //  All references to variables and functions by name, should get pointers to the symbols in the table.
    //  This should performed by bind_names( function symbol table, function body AST node )

    //  It also handles adding local variables to the local symbol table, and pushing and popping scopes.
    //  A final task performed by bind_names(), is adding strings to the global string list
}

/**
 * Prints the global symbol table, and the local symbol tables for each function.
 * Also prints the global string list.
 * Finally prints out the AST again, with bound symbols.
 */
void print_tables(void) {
    print_symbol_table(global_symbols, 0);
    printf("\n == STRING LIST == \n");
    print_string_list();
    printf("\n == BOUND SYNTAX TREE == \n");
    print_syntax_tree();
}

/* Destroys all symbol tables and the global string list */
void destroy_tables(void) {
    destroy_symbol_tables();
    destroy_string_list();
}

/* Internal matters */

/**
 * Goes through all global declarations in the syntax tree, adding them to the global symbol table.
 * When adding functions, local symbol tables are created, and symbols for the functions parameters are added.
 *
 * Create symbols for all global defintions (global variables, arrays and functions), and add them to the global symbol table.
 * Functions can also get their local symbol tables created here, and symbols for all its parameters
 */
static void find_globals(void) {
    global_symbols = symbol_table_init();

    // For each top level node in the syntax tree, create symbols for global variables, arrays and functions.
    for (size_t i = 0; i < root->n_children; i++) {
        node_t *node = root->children[i];
        switch (node->type) {
            case DECLARATION: {
                assert(node->n_children > 0);
                // Iterate through all global variables, and add them to the global symbol table
                for (size_t j = 0; j < node->n_children; j++) {
                    node_t *child = node->children[j];
                    if (child->type == IDENTIFIER_DATA) {
                        symbol_t *global_variable_symbol = malloc(sizeof(symbol_t));
                        global_variable_symbol->name = child->data;
                        global_variable_symbol->type = SYMBOL_GLOBAL_VAR;
                        symbol_table_insert(global_symbols, global_variable_symbol);
                    }
                }
            }

            case ARRAY_DECLARATION: {
                assert(node->n_children == 2);
                // Iterate through all global arrays, and add them to the global symbol table
                for (size_t j = 0; j < node->n_children; j++) {
                    node_t *child = node->children[j];
                    if (child->type == ARRAY_DECLARATION) {
                        symbol_t *global_array_symbol = malloc(sizeof(symbol_t));
                        global_array_symbol->name = child->data;
                        global_array_symbol->type = SYMBOL_GLOBAL_ARRAY;
                        symbol_table_insert(global_symbols, global_array_symbol);
                    }
                }
            }
            case FUNCTION: {
                assert(node->n_children == 3);
                // Iterate through all functions, and add them to the global symbol table
                symbol_t *function_symbol = malloc(sizeof(symbol_t));
                node_t *symbol_name = node->children[0];
                function_symbol->name = symbol_name->data;
                function_symbol->type = SYMBOL_FUNCTION;
                symbol_table_insert(global_symbols, function_symbol);

                // Create a local symbol table for the function, and add all its parameters and local variables to it
                function_symbol->function_symtable = symbol_table_init();
                node_t *parameter_list = node->children[1];
                for (size_t j = 0; j < parameter_list->n_children; j++) {
                    node_t *identifier = parameter_list->children[j];
                    assert(identifier->type == IDENTIFIER_DATA);

                    symbol_t *parameter_symbol = malloc(sizeof(symbol_t));
                    parameter_symbol->name = identifier->data;
                    parameter_symbol->type = SYMBOL_PARAMETER;
                    symbol_table_insert(function_symbol->function_symtable, parameter_symbol);
                }

                // TODO: Bind names
                node_t *function_body = node->children[2];
                bind_names(function_symbol->function_symtable, function_body);
            }
        }
    }
}

/**
 * A recursive function that traverses the body of a function, and:
 *  - Adds variable declarations to the function's local symbol table.
 *  - Pushes and pops local variable scopes when entering blocks.
 *  - Binds identifiers to the symbol it references.
 *  - Inserts STRING_DATA nodes' data into the global string list, and replaces it with its list position.
 */
static void bind_names(symbol_table_t *local_symbols, node_t *node) {
    // TODO: Implement bind_names, doing all the things described above
    // Tip: See symbol_hashmap_init () in symbol_table.h, to make new hashmaps for new scopes.
    // Remember the symbol_hashmap_t's backup pointer, allowing you to make linked lists.
}

/** Prints the given symbol table, with sequence number, symbol names and types.
 * When printing function symbols, its local symbol table is recursively printed, with indentation.
 */
static void print_symbol_table(symbol_table_t *table, int nesting) {
    // TODO: Output the given symbol table
    // TIP: Use SYMBOL_TYPE_NAMES[ my_sybmol->type ] to get a human readable string for each symbol type

    for (size_t i = 0; i < table->n_symbols; i++) {
        symbol_t *symbol = table->symbols[i];
        printf("%*s %s: %s", nesting, "", symbol->name, SYMBOL_TYPE_NAMES[symbol->type]);

        if (symbol->type == SYMBOL_FUNCTION) {
            printf("");
        }
    }
}

/* Frees up the memory used by the global symbol table, all local symbol tables, and their symbols */
static void destroy_symbol_tables(void) {
    // TODO: Implement cleanup. All symbols in the program are owned by exactly one symbol table.
}

/** Adds the given string to the global string list, resizing if needed.
 * Takes ownership of the string, and returns its position in the string list.
 */
static size_t add_string(char *string) {
    // TODO: Write a helper function you can use during bind_names(),
    // to easily add a string into the dynamically growing string_list
}

/* Prints all strings added to the global string list */
static void print_string_list(void) {
    // TODO: Implement printing of the string list like so:
    // 0: "string 1"
    // 1: "some other string"
}

/* Frees all strings in the global string list, and the string list itself */
static void destroy_string_list(void) {
    // TODO: Called during cleanup, free strings, and the memory used by the string list itself
}

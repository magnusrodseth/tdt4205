#include <vslc.h>

/* Global symbol table and string list */
symbol_table_t *global_symbols;
char **string_list;
size_t string_list_len;
size_t string_list_capacity;

static void find_global_declaration(node_t *node);
static void find_global_array_declaration(node_t *node);
static void find_global_function(node_t *node);
static void find_globals(void);

static void bind();
static void bind_declaration(symbol_table_t *local_symbols, node_t *node);
static void bind_identifiers_to_symbols(symbol_table_t *local_symbols, node_t *node);
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
    find_globals();
    bind();
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
 * @brief Binds all identifiers in a function body to their symbol table entries.
 *
 * Once all global symbols are added, go through all function bodies.
 * All references to variables and functions by name, should get pointers to the symbols in the table.
 * This should be performed by bind_names(function symbol table, function body AST node)
 * It also handles adding local variables to the local symbol table, and pushing and popping scopes.
 * A final task performed by bind_names(), is adding strings to the global string list
 */
static void bind() {
    for (size_t i = 0; i < root->n_children; i++) {
        node_t *node = root->children[i];
        if (node->type == FUNCTION) {
            assert(node->n_children == 3);
            node_t *function_body = node->children[2];
            assert(node->symbol->function_symtable != NULL);
            bind_names(node->symbol->function_symtable, function_body);
        }
    }
}

static void find_global_declaration(node_t *node) {
    assert(node->n_children > 0);
    assert(node->type == DECLARATION);

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

static void find_global_array_declaration(node_t *node) {
    assert(node->n_children == 2);
    assert(node->type == ARRAY_DECLARATION);

    symbol_t *global_array_symbol = malloc(sizeof(symbol_t));
    node_t *identifier = node->children[0];
    global_array_symbol->name = identifier->data;
    global_array_symbol->type = SYMBOL_GLOBAL_ARRAY;
    symbol_table_insert(global_symbols, global_array_symbol);
}

static void find_global_function(node_t *node) {
    assert(node->n_children == 3);
    assert(node->type == FUNCTION);

    symbol_t *function_symbol = malloc(sizeof(symbol_t));
    node_t *identifier = node->children[0];
    function_symbol->name = identifier->data;
    function_symbol->type = SYMBOL_FUNCTION;
    symbol_table_insert(global_symbols, function_symbol);

    // Create a local symbol table for the function, and add all its parameters to it
    function_symbol->function_symtable = symbol_table_init();
    node_t *parameter_list = node->children[1];
    for (size_t j = 0; j < parameter_list->n_children; j++) {
        node_t *parameter = parameter_list->children[j];
        assert(parameter->type == IDENTIFIER_DATA);
        symbol_t *parameter_symbol = malloc(sizeof(symbol_t));
        parameter_symbol->name = parameter->data;
        parameter_symbol->type = SYMBOL_PARAMETER;
        symbol_table_insert(function_symbol->function_symtable, parameter_symbol);
    }

    node->symbol = function_symbol;
}

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
            case DECLARATION:
                find_global_declaration(node);
                break;
            case ARRAY_DECLARATION:
                find_global_array_declaration(node);
                break;
            case FUNCTION:
                find_global_function(node);
            default:
                break;
        }
    }
}

/**
 * @brief Binds all identifiers in a function body to their symbol table entries.
 *
 * Performs a recursive traversal of the function body.
 *
 * @param local_symbols is the symbol table for the function.
 * @param node is the current node in the traversal.
 */
static void bind_declaration(symbol_table_t *local_symbols, node_t *node) {
    for (size_t i = 0; i < node->n_children; i++) {
        node_t *child = node->children[i];

        if (child->type == IDENTIFIER_DATA) {
            symbol_t *local_variable_symbol = malloc(sizeof(symbol_t));
            local_variable_symbol->name = child->data;
            local_variable_symbol->type = SYMBOL_LOCAL_VAR;
            symbol_table_insert(local_symbols, local_variable_symbol);
        }

        bind_declaration(local_symbols, child);
    }
}

static void bind_identifiers_to_symbols(symbol_table_t *local_symbols, node_t *node) {
    for (size_t j = 0; j < node->n_children; j++) {
        node_t *second_child = node->children[j];
        if (second_child->type == IDENTIFIER_DATA) {
            symbol_t *symbol = symbol_hashmap_lookup(local_symbols->hashmap, second_child->data);
            if (symbol == NULL) {
                symbol = symbol_hashmap_lookup(global_symbols->hashmap, second_child->data);
            }

            second_child->symbol = symbol;
        }
    }
}

static void add_string_to_global_list(node_t *node) {
    size_t position = add_string(node->data);
    int64_t *data = malloc(sizeof(int64_t));
    *data = position;
    node->data = data;
}

/**
 * A recursive function that traverses the body of a function, and:
 *  - Adds variable declarations to the function's local symbol table.
 *  - Pushes and pops local variable scopes when entering blocks.
 *  - Binds identifiers to the symbol it references.
 *  - Inserts STRING_DATA nodes' data into the global string list, and replaces it with its list position.
 */
static void bind_names(symbol_table_t *local_symbols, node_t *node) {
    // Recursively traverse the body of the function (from the provided node)
    for (size_t i = 0; i < node->n_children; i++) {
        node_t *child = node->children[i];

        switch (child->type) {
            case DECLARATION:
                bind_declaration(local_symbols, child);
                break;
            case ASSIGNMENT_STATEMENT:
            case EXPRESSION:
            case RELATION:
            case PRINT_STATEMENT:
                bind_identifiers_to_symbols(local_symbols, child);
                break;
            case BLOCK: {
                symbol_hashmap_t *new_scope = symbol_hashmap_init();
                // Insert new hashmap into the linked list
                new_scope->backup = local_symbols->hashmap;
                local_symbols->hashmap = new_scope;
                break;
            }
            case STRING_DATA:
                add_string_to_global_list(child);
                break;
            default:
                break;
        }

        bind_names(local_symbols, child);
    }
}

/**
 * Prints the given symbol table, with sequence number, symbol names and types.
 * When printing function symbols, its local symbol table is recursively printed, with indentation.
 */
static void print_symbol_table(symbol_table_t *table, int nesting) {
    int max_num_digits = snprintf(NULL, 0, "%d", table->n_symbols - 1);

    for (int i = 0; i < table->n_symbols; i++) {
        symbol_t *symbol = table->symbols[i];

        if (symbol != NULL) {
            // Ensure the symbol table is printed with correct indentation
            printf("%*s%*d: %s(%s)\n", nesting, "", max_num_digits, i, SYMBOL_TYPE_NAMES[symbol->type], symbol->name);

            if (symbol->type == SYMBOL_FUNCTION) {
                print_symbol_table(symbol->function_symtable, nesting + 4);
            }
        }
    }
}

/* Frees up the memory used by the global symbol table, all local symbol tables, and their symbols */
static void destroy_symbol_tables(void) {
    for (size_t i = 0; i < global_symbols->n_symbols; i++) {
        symbol_t *symbol = global_symbols->symbols[i];
        if (symbol != NULL) {
            if (symbol->type == SYMBOL_FUNCTION) {
                symbol_table_destroy(symbol->function_symtable);
            }
        }
    }

    symbol_table_destroy(global_symbols);
}

/**
 * Adds the given string to the global string list, resizing if needed.
 * Takes ownership of the string, and returns its position in the string list.
 */
static size_t add_string(char *string) {
    // If the string list is full, resize it
    if (string_list_len == string_list_capacity) {
        string_list_capacity *= 2;
        string_list = realloc(string_list, string_list_capacity * sizeof(char *));
    }

    // Add the string to the list, and return its position
    string_list[string_list_len] = string;
    size_t position = string_list_len;
    string_list_len++;

    return position;
}

/* Prints all strings added to the global string list */
static void print_string_list(void) {
    for (int i = 0; i < string_list_len; i++) {
        printf("%d: %s\n", i, string_list[i]);
    }
}

/* Frees all strings in the global string list, and the string list itself */
static void destroy_string_list(void) {
    for (int i = 0; i < string_list_len; i++) {
        free(string_list[i]);
    }
    free(string_list);
}

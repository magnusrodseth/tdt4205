#define NODETYPES_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>
#include <vslc.h>

/* Global root for parse tree and abstract syntax tree */
node_t *root;

typedef enum ArithmeticOperator {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    NONE
} ArithmeticOperator;

static void node_print(node_t *node, int nesting);
static void node_finalize(node_t *discard);
static void destroy_subtree(node_t *discard);
static node_t *simplify_tree(node_t *node);
static node_t *replace_with_child(node_t *node);
static node_t *squash_child(node_t *node);
static node_t *flatten_list(node_t *node);
static bool all_children_are_numbers(node_t *node);
static ArithmeticOperator string_to_arithmetic_operator(char *string);
static node_t *constant_fold_expression(node_t *node);
static node_t *fold_expression(node_t *node);
static void calculate_unary_fold(node_t *node, int64_t *result);
static void calculate_binary_fold(node_t *node, int64_t *result);
static node_t *replace_for_statement(node_t *for_node);

/* External interface */
void print_syntax_tree() {
    if (getenv("GRAPHVIZ_OUTPUT") != NULL)
        graphviz_node_print(root);
    else
        node_print(root, 0);
}

void simplify_syntax_tree(void) {
    root = simplify_tree(root);
}

void destroy_syntax_tree(void) {
    destroy_subtree(root);
    root = NULL;
}

/* Initialize a node with type, data, and children */
void node_init(node_t *node, node_type_t type, void *data, uint64_t n_children, ...) {
    node->type = type;
    node->data = data;
    node->n_children = n_children;
    node->symbol = NULL;

    va_list args;
    va_start(args, n_children);
    node->children = malloc(n_children * sizeof(node_t *));

    for (int i = 0; i < n_children; i++) {
        node->children[i] = va_arg(args, node_t *);
    }

    va_end(args);

    if (root == NULL) {
        root = node;
    }
}

/* Inner workings */
/* Prints out the given node and all its children recursively */
static void node_print(node_t *node, int nesting) {
    if (node != NULL) {
        printf("%*s%s", nesting, "", node_strings[node->type]);
        if (node->type == IDENTIFIER_DATA ||
            node->type == EXPRESSION ||
            node->type == RELATION)
            printf("(%s)", (char *)node->data);
        else if (node->type == NUMBER_DATA)
            printf("(%ld)", *(int64_t *)node->data);
        else if (node->type == STRING_DATA) {
            if (node->data && *(char *)node->data != '"')
                printf("(#%ld)", *(int64_t *)node->data);
            else
                printf("(%s)", (char *)node->data);
        }

        // If the node has a symbol, print that as well
        if (node->symbol)
            printf(" %s(%ld)",
                   ((const char *[]){[SYMBOL_GLOBAL_VAR] = "GLOBAL_VAR",
                                     [SYMBOL_GLOBAL_ARRAY] = "GLOBAL_ARRAY",
                                     [SYMBOL_FUNCTION] = "FUNCTION",
                                     [SYMBOL_PARAMETER] = "PARAMETER",
                                     [SYMBOL_LOCAL_VAR] =
                                         "LOCAL_VAR"})[node->symbol->type],
                   node->symbol->sequence_number);

        putchar('\n');
        for (int64_t i = 0; i < node->n_children; i++)
            node_print(node->children[i], nesting + 1);
    } else
        printf("%*s(NULL)\n", nesting, "");
}

/* Frees the memory owned by the given node, but does not touch its children */
static void node_finalize(node_t *discard) {
    if (discard->data != NULL) {
        free(discard->data);
    }

    if (discard->children != NULL) {
        free(discard->children);
    }

    free(discard);
}

/* Recursively frees the memory owned by the given node, and all its children */
static void destroy_subtree(node_t *discard) {
    if (discard == NULL) {
        return;
    }

    if (discard->n_children == 0) {
        node_finalize(discard);
        return;
    }

    for (int i = 0; i < discard->n_children; i++) {
        destroy_subtree(discard->children[i]);
    }

    node_finalize(discard);

    if (discard == root) {
        root = NULL;
    }
}

/* Recursive function to convert a parse tree into an abstract syntax tree */
static node_t *simplify_tree(node_t *node) {
    if (node == NULL) {
        return NULL;
    }

    // Simplify everything in the node's subtree before proceeding
    for (uint64_t i = 0; i < node->n_children; i++)
        node->children[i] = simplify_tree(node->children[i]);

    switch (node->type) {
        case PROGRAM:
        case GLOBAL:
        case PRINT_ITEM:
        case STATEMENT:
            return replace_with_child(node);

        case VARIABLE_LIST:
        case PRINT_LIST:
        case STATEMENT_LIST:
        case GLOBAL_LIST:
        case DECLARATION_LIST:
        case EXPRESSION_LIST:
            return flatten_list(node);

        case PRINT_STATEMENT:
        case DECLARATION:
        case PARAMETER_LIST:
        case ARRAY_DECLARATION:
        case ARGUMENT_LIST:
            return squash_child(node);

        case EXPRESSION:
            return constant_fold_expression(node);

        case FOR_STATEMENT:
            return replace_for_statement(node);

        default:
            break;
    }

    return node;
}

// Helper macros for manually building an AST
#define NODE(variable_name, ...)                    \
    node_t *variable_name = malloc(sizeof(node_t)); \
    node_init(variable_name, __VA_ARGS__)
// After an IDENTIFIER_NODE has been added to the tree, it can't be added again
// This macro replaces the given variable with a new node, containting a copy of the data
#define DUPLICATE_VARIABLE(variable)                         \
    do {                                                     \
        char *identifier = strdup(variable->data);           \
        variable = malloc(sizeof(node_t));                   \
        node_init(variable, IDENTIFIER_DATA, identifier, 0); \
    } while (false)
#define FOR_END_VARIABLE "__FOR_END__"

/**
 * @brief Replaces a node with its only child, letting the child take over the parent's position.
 *
 * @param node is the node to be replaced.
 * @return node_t* the root of the new subtree.
 */
static node_t *replace_with_child(node_t *node) {
    assert(node->n_children == 1);
    node_t *child = node->children[0];
    node_finalize(node);
    return child;
}

static node_t *squash_child(node_t *node) {
    assert(node->n_children <= 1);

    if (node->n_children == 1) {
        node_t *result = node->children[0];
        result->type = node->type;
        node_finalize(node);
        return result;
    }

    return node;
}

static node_t *flatten_list(node_t *node) {
    assert(node->n_children <= 2);

    if (node->n_children == 0 || node->n_children == 1) {
        return node;
    }

    node_t *left = node->children[0];
    node_t *right = node->children[1];

    if (left->type == node->type) {
        // Flatten left child
        node->children = realloc(node->children, sizeof(node_t *) * (left->n_children + 1));
        for (uint64_t i = 0; i < left->n_children; i++) {
            node->children[i] = left->children[i];
        }
        node->children[left->n_children] = right;
        node->n_children = left->n_children + 1;

        node_finalize(left);
    }

    return node;
}

/**
 * @brief Wrapper for performing constant folding on either a unary or binary expression.
 *
 * @param node is the expression node.
 */
static node_t *fold_expression(node_t *node) {
    assert(node->n_children == 1 || node->n_children == 2);

    int64_t *result = malloc(sizeof(int64_t));
    *result = 0;

    if (node->n_children == 1) {
        calculate_unary_fold(node, result);
    } else if (node->n_children == 2) {
        calculate_binary_fold(node, result);
    }

    node->type = NUMBER_DATA;
    node->data = result;
    node->n_children = 0;

    return node;
}

/**
 * @brief Performs constant folding on an expression with only one child.
 *
 * @param node is the expression node.
 * @param result is the result of the constant folding.
 **/
static void calculate_unary_fold(node_t *node, int64_t *result) {
    ArithmeticOperator operator= string_to_arithmetic_operator(node->data);

    int64_t *child_value = node->children[0]->data;
    switch (operator) {
        case ADD:
            *result = *child_value;
            break;
        case SUBTRACT:
            *result = -(*child_value);
            break;
        case MULTIPLY:
            *result = 0;
            break;
        case DIVIDE:
            *result = 0;
            break;
        default:
            break;
    }
}

/**
 * @brief Performs constant folding on an expression with two children.
 *
 * @param node is the expression node.
 * @param result is the result of the constant folding.
 */
static void calculate_binary_fold(node_t *node, int64_t *result) {
    ArithmeticOperator operator= string_to_arithmetic_operator(node->data);

    int64_t *left = node->children[0]->data;
    int64_t *right = node->children[1]->data;

    switch (operator) {
        case ADD:
            *result = (*left) + (*right);
            break;
        case SUBTRACT:
            *result = (*left) - (*right);
            break;
        case MULTIPLY:
            *result = (*left) * (*right);
            break;
        case DIVIDE:
            *result = (*left) / (*right);
            break;
        default:
            break;
    }
}

static node_t *constant_fold_expression(node_t *node) {
    assert(node->type == EXPRESSION);
    assert(node->n_children <= 2);

    bool is_operator = node->data != NULL;

    // Expressions with no operator and one child are only wrappers, and can be replaced by their children
    if (!is_operator && node->n_children == 1) {
        return replace_with_child(node);
    }

    // Expressions with operators can have 1 or 2 children,
    // and we can only do constant folding if all children are numbers
    if (is_operator && node->n_children > 0) {
        if (all_children_are_numbers(node)) {
            return fold_expression(node);
        }
    }

    return node;
}

/**
 * @brief Replaces a FOR_STATEMENT with a WHILE_STATEMENT.
 *
 * The returned BLOCK node contains variables, setup and a while loop.
 *
 * @param for_node is the root node of the FOR_STATEMENT.
 * @return node_t* is the root node of the WHILE_STATEMENT, which will always be a BLOCK node.
 */
static node_t *replace_for_statement(node_t *for_node) {
    assert(for_node->type == FOR_STATEMENT);

    node_t *variable = for_node->children[0];
    node_t *start_value = for_node->children[1];
    node_t *end_value = for_node->children[2];
    node_t *body = for_node->children[3];

    node_finalize(for_node);

    // Make the declaration for both variables
    // var <variable>, __FOR_END__
    NODE(end_variable, IDENTIFIER_DATA, strdup(FOR_END_VARIABLE), 0);
    NODE(declaration, DECLARATION, NULL, 2, variable, end_variable);
    NODE(declaration_list, DECLARATION_LIST, NULL, 1, declaration);

    // make the assignments
    // <variable> := <start_value>
    // __FOR_END__ := <end_value>
    DUPLICATE_VARIABLE(variable);
    NODE(init_assignment, ASSIGNMENT_STATEMENT, NULL, 2, variable, start_value);
    DUPLICATE_VARIABLE(end_variable);
    NODE(end_assignment, ASSIGNMENT_STATEMENT, NULL, 2, end_variable, end_value);

    // make the relation
    // <variable> < __FOR_END__
    DUPLICATE_VARIABLE(variable);
    DUPLICATE_VARIABLE(end_variable);
    NODE(relation, RELATION, strdup("<"), 2, variable, end_variable);

    // make the increment statement
    // <variable> := <variable> + 1
    DUPLICATE_VARIABLE(variable);
    int64_t *one = malloc(sizeof(int64_t));
    *one = 1;
    NODE(one_node, NUMBER_DATA, one, 0);
    NODE(variable_plus_one, EXPRESSION, strdup("+"), 2, variable, one_node);
    DUPLICATE_VARIABLE(variable);
    NODE(increment, ASSIGNMENT_STATEMENT, NULL, 2, variable, variable_plus_one);

    // make a block statement containing both the original for-loop body, and the
    // increment begin
    //     <body>
    //     <variable> := <variable> + 1
    // end
    NODE(inner_statement_list, STATEMENT_LIST, NULL, 2, body, increment);
    NODE(inner_block, BLOCK, NULL, 1, inner_statement_list);

    // Make the while loop like so:
    // while <variable> < __FOR_END__ begin
    //     <body>
    //     <variable> := <variable> + 1
    // end
    NODE(while_node, WHILE_STATEMENT, NULL, 2, relation, inner_block);

    // Put it all together into a statement list
    // <variable> := <start_value>
    // __FOR_END__ := <end_value>
    // while <variable> < __FOR_END__ begin
    //     <body>
    //     <variable> := <variable> + 1
    // end
    NODE(result_statement_list, STATEMENT_LIST, NULL, 3, init_assignment,
         end_assignment, while_node);

    // Include the declaration of the two local variables
    NODE(result, BLOCK, NULL, 2, declaration_list, result_statement_list);

    return result;
}

static bool all_children_are_numbers(node_t *node) {
    for (uint64_t i = 0; i < node->n_children; i++) {
        if (node->children[i]->type != NUMBER_DATA) {
            return false;
        }
    }
    return true;
}

static ArithmeticOperator string_to_arithmetic_operator(char *string) {
    if (string == NULL) {
        return NONE;
    }

    if (strcmp(string, "+") == 0) {
        return ADD;
    } else if (strcmp(string, "-") == 0) {
        return SUBTRACT;
    } else if (strcmp(string, "*") == 0) {
        return MULTIPLY;
    } else if (strcmp(string, "/") == 0) {
        return DIVIDE;
    } else {
        return NONE;
    }
}
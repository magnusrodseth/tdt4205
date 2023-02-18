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

// Tasks
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
            node->type == STRING_DATA ||
            node->type == EXPRESSION ||
            node->type == RELATION)
            printf("(%s)", (char *)node->data);
        else if (node->type == NUMBER_DATA)
            printf("(%ld)", *((int64_t *)node->data));
        putchar('\n');
        for (int64_t i = 0; i < node->n_children; i++)
            node_print(node->children[i], nesting + 1);
    } else
        printf("%*s%p\n", nesting, "", node);
}

/* Frees the memory owned by the given node, but does not touch its children */
static void node_finalize(node_t *discard) {
    if (discard->data != NULL) {
        free(discard->data);
    }

    if (discard->symbol != NULL) {
        free(discard->symbol);
    }

    if (discard->children != NULL) {
        free(discard->children);
    }

    free(discard);
}

/* Recursively frees the memory owned by the given node, and all its children */
static void destroy_subtree(node_t *discard) {
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
            return flatten_list(node);

        case PRINT_STATEMENT:
        case DECLARATION:
        case PARAMETER_LIST:
            return squash_child(node);

        // Do contstant folding, if possible
        // Also prunes expressions that are just wrapping atomic expressions
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

    if (node->n_children == 0) {
        return node;
    }

    // Link child's children to node
    node_t *child = node->children[0];

    if (child->n_children == 0) {
        return node;
    }

    node->children = realloc(node->children, sizeof(node_t *) * child->n_children);
    for (uint64_t i = 0; i < child->n_children; i++) {
        node->children[i] = child->children[i];
    }
    node->n_children = child->n_children;

    node_finalize(child);

    return node;
}

static node_t *flatten_list(node_t *node) {
    assert(node->n_children <= 2);

    if (node->n_children == 0) {
        return node;
    }

    if (node->n_children == 1) {
        return replace_with_child(node);
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

static node_t *constant_fold_expression(node_t *node) {
    assert(node->type == EXPRESSION);
    assert(node->n_children <= 2);

    bool is_operator = node->data != NULL;

    // First, expression nodes with no operator, and only one child,
    // are just wrappers for NUMBER_DATA, IDENTIFIER_DATA or ARRAY_INDEXING,
    // and can be replaced by its children.
    if (!is_operator && node->n_children == 1) {
        return replace_with_child(node);
    }

    // For expression nodes that are operators, we can only do constant folding if all its children are NUMBER_DATA.
    // In such cases, the expression node can be replaced with the value of its operator, applied to its child(ren).
    if (is_operator && node->n_children > 0) {
        if (all_children_are_numbers(node)) {
            // Replace expression node with the value of its operator, applied to its children
            ArithmeticOperator operator= string_to_arithmetic_operator(node->data);
            // TODO
        }

        // Remember to free up the memory used by the original node(s), if they get replaced by a new node
        return node;
    }
}

// Replaces the FOR_STATEMENT with a BLOCK.
// The block contains varables, setup, and a while loop
static node_t *replace_for_statement(node_t *for_node) {
    assert(for_node->type == FOR_STATEMENT);

    // extract child nodes from the FOR_STATEMENT
    node_t *variable = for_node->children[0];
    node_t *start_value = for_node->children[1];
    node_t *end_value = for_node->children[2];
    node_t *body = for_node->children[3];

    // TODO: Task 2.4
    // Replace the FOR_STATEMENT node, by instead creating the syntax tree of an equivalent block with a while-statement
    // As an example, the following statement:
    //
    // for i in 5..N+1
    //     print a[i]
    //
    // should become:
    //
    // begin
    //     var i, __FOR_END__
    //     i := 5
    //     __FOR_END__ := N+1
    //     while i < __FOR_END__ begin
    //         print a[i]
    //         i := i + 1
    //     end
    // end
    //

    // To aid in the manual creation of AST nodes, you can create named nodes using the NODE macro
    // As an example, the following creates the
    // var <variable>, __FOR_END__
    // part of the transformation
    NODE(end_variable, IDENTIFIER_DATA, strdup(FOR_END_VARIABLE), 0);
    NODE(variable_list, VARIABLE_LIST, NULL, 2, variable, end_variable);
    NODE(declaration, DECLARATION, NULL, 1, variable_list);
    NODE(declaration_list, DECLARATION_LIST, NULL, 1, declaration);

    // An important thing to note, is that nodes may not be re-used
    // since that will cause errors when freeing up the syntax tree later.
    // because we want to use a lot of IDENTIFIER_DATA nodes with the same data, we have the macro
    DUPLICATE_VARIABLE(variable);
    // Now we can use <variable> again, and it will be a new node for the same identifier!
    NODE(init_assignment, ASSIGNMENT_STATEMENT, NULL, 2, variable, start_value);
    // We do the same whenever we want to reuse <end_variable> as well
    DUPLICATE_VARIABLE(end_variable);
    NODE(end_assignment, ASSIGNMENT_STATEMENT, NULL, 2, end_variable, end_value);

    // TODO: The rest is up to you. Good luck!
    // Don't fret if this part gets too cumbersome. Try your best

    // TODO: Instead of returning the original for_node, destroy it, and return your equivalent block
    return for_node;
}

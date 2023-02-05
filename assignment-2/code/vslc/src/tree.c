#define NODETYPES_IMPLEMENTATION

#include <vslc.h>

/* Global root for parse tree and abstract syntax tree */
node_t *root;

// Tasks
static void node_print(node_t *node, int nesting);

static void node_finalize(node_t *discard);

static void destroy_subtree(node_t *discard);

/* External interface */
void print_syntax_tree(void)
{
    node_print(root, 0);
}

void destroy_syntax_tree(void)
{
    // This will destroy the entire tree by deleting root and all its children
    destroy_subtree(root);
}

/* Inner workings */
/* Prints out the given node and all its children recursively */
static void node_print(node_t *node, int nesting)
{
    if (node != NULL)
    {
        printf("%*s%s", nesting, "", node_strings[node->type]);
        if (node->type == IDENTIFIER_DATA ||
            node->type == STRING_DATA ||
            node->type == EXPRESSION)
            printf("(%s)", (char *)node->data);
        else if (node->type == NUMBER_DATA)
            printf("(%ld)", *((int64_t *)node->data));
        putchar('\n');
        for (int64_t i = 0; i < node->n_children; i++)
            node_print(node->children[i], nesting + 1);
    }
    else
        printf("%*s%p\n", nesting, "", node);
}

/* Initialize a node with type, data, and children */
void node_init(node_t *node, node_type_t type, void *data, uint64_t n_children, ...)
{
    node->type = type;
    node->data = data;
    node->n_children = n_children;
    node->symbol = NULL;

    va_list args;
    va_start(args, n_children);
    node->children = malloc(n_children * sizeof(node_t *));

    for (int i = 0; i < n_children; i++)
    {
        node->children[i] = va_arg(args, node_t *);
    }

    va_end(args);

    if (root == NULL)
    {
        root = node;
    }
}

/* Frees the memory owned by the given node, but does not touch its children */
static void node_finalize(node_t *discard)
{
    if (discard->data != NULL)
    {
        free(discard->data);
    }

    if (discard->symbol != NULL)
    {
        free(discard->symbol);
    }

    if (discard->children != NULL)
    {
        free(discard->children);
    }

    free(discard);
}

/* Frees the memory owned by the given node, and all its children */
static void destroy_subtree(node_t *discard)
{
    if (discard->n_children == 0)
    {
        node_finalize(discard);
        return;
    }

    for (int i = 0; i < discard->n_children; i++)
    {
        destroy_subtree(discard->children[i]);
    }

    node_finalize(discard);

    if (discard == root)
    {
        root = NULL;
    }
}

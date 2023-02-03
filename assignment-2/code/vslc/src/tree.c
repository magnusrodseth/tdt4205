#define NODETYPES_IMPLEMENTATION
#include <vslc.h>

/* Global root for parse tree and abstract syntax tree */
node_t *root;

// Tasks
static void node_print ( node_t *node, int nesting );
static void node_finalize ( node_t *discard );
static void destroy_subtree ( node_t *discard );

/* External interface */
void print_syntax_tree ( void )
{
    node_print ( root, 0 );
}

void destroy_syntax_tree ( void )
{
    // This will destroy the enitre tree by deleting root and all its children
    destroy_subtree ( root );
}

/* Inner workings */
/* Prints out the given node and all its children recursively */
static void node_print ( node_t *node, int nesting )
{
    if ( node != NULL )
    {
        printf ( "%*s%s", nesting, "", node_strings[node->type] );
        if ( node->type == IDENTIFIER_DATA ||
             node->type == STRING_DATA ||
             node->type == EXPRESSION )
            printf ( "(%s)", (char *) node->data );
        else if ( node->type == NUMBER_DATA )
            printf ( "(%ld)", *((int64_t *)node->data) );
        putchar ( '\n' );
        for ( int64_t i=0; i<node->n_children; i++ )
            node_print ( node->children[i], nesting+1 );
    }
    else
        printf ( "%*s%p\n", nesting, "", node );
}

/* Initialize a node with type, data, and children */ 
void node_init ( node_t *nd, node_type_t type, void *data, uint64_t n_children, ... )
{
    /* 
       TODO: 
       Initializer function for a syntax tree node 
       
       HINT:
       Initialize the pre-allocated `nd` node with type, data, and children.
       See include/tree.h for the node_t struct
       Node has no `symbol` on initialization, so this should be set to NULL.
       Remember to *allocate* space for the node's children.
    */
}

/* Frees the memory owned by the given node, but does not touch its children */
static void node_finalize ( node_t *discard )
{
    /* 
       TODO: 
       Remove memory allocated for a single syntax tree node.

       HINT:
       *Free* all data members owned by this node - including the memory used by the node.
       Only free the memory occupied by this - do not touch its children.
    */
}

/* Frees the memory owned by the given node, and all its children */
static void destroy_subtree ( node_t *discard )
{
    /* 
       TODO: 
       Remove all nodes in the subtree rooted at a node.

       HINT:
       Destroy entire *trees* instead of single *nodes*.
       Seems like you can use the `node_finalize` function in some way here...        
    */
}

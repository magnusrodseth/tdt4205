#ifndef NODETYPES_H
#define NODETYPES_H

// This file is a bit magic
// When #include-d normally, it defines the enum node_type_t,
// with one entry for each node type (e.g. PROGRAM, GLOBAL_LIST etc.).
// It also exports the string-array node_strings, containing the names
// of each node type, in the same order they appear in node_type_t.
// This allows printing node_type_t values as their node name like so:
//
// printf("my_node is of type %s\n", node_strings[my_node->type]);
//
// When this file is included like so:
//     #define NODETYPES_IMPLEMENTATION
//     #include "nodetypes.h"
// it defines the string-array node_strings. This should only be done in one .c-file

#ifdef NODETYPES_IMPLEMENTATION
#define NODELIST_BEGIN const char *node_strings[] = {
#define NODE(name) #name
#define NODELIST_END };
#else
#define NODELIST_BEGIN typedef enum {
#define NODE(name) name
#define NODELIST_END , _NODE_COUNT } node_type_t; \
extern const char *node_strings[_NODE_COUNT];
#endif // NODETYPES_IMPLEMENTATION

NODELIST_BEGIN
    NODE(PROGRAM),
    NODE(GLOBAL_LIST),
    NODE(GLOBAL),
    NODE(DECLARATION),
    NODE(VARIABLE_LIST),
    NODE(VARIABLE),
    NODE(ARRAY_DECLARATION),
    NODE(ARRAY_INDEXING),
    NODE(FUNCTION),
    NODE(PARAMETER_LIST),
    NODE(STATEMENT),
    NODE(BLOCK),
    NODE(DECLARATION_LIST),
    NODE(STATEMENT_LIST),
    NODE(ASSIGNMENT_STATEMENT),
    NODE(RETURN_STATEMENT),
    NODE(PRINT_STATEMENT),
    NODE(PRINT_LIST),
    NODE(PRINT_ITEM),
    NODE(BREAK_STATEMENT),
    NODE(IF_STATEMENT),
    NODE(WHILE_STATEMENT),
    NODE(RELATION),
    NODE(FOR_STATEMENT),
    NODE(ARGUMENT_LIST),
    NODE(EXPRESSION_LIST),
    NODE(EXPRESSION),
    NODE(IDENTIFIER_DATA),
    NODE(NUMBER_DATA),
    NODE(STRING_DATA)
NODELIST_END

#undef NODELIST_BEGIN
#undef NODE
#undef NODELIST_END

// If we just defined the node_strings array instead of enum node_type_t
// run once again without NODETYPES_IMPLEMENTATION defined, to get both.
#ifdef NODETYPES_IMPLEMENTATION
#undef NODETYPES_IMPLEMENTATION
#undef NODETYPES_H
#include "nodetypes.h"
#endif

#endif // NODETYPES_H

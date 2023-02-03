%{
#include <vslc.h>

/* State variables from the flex generated scanner */
extern int yylineno; // The line currently being read
extern char yytext[]; // The text of the last consumed lexeme
/* The main flex driver function used by the parser */
int yylex ( void );
/* The function called by the parser when errors occur */
int yyerror ( const char *error )
{
    fprintf ( stderr, "%s on line %d\n", error, yylineno );
    exit ( EXIT_FAILURE );
}

%}

%left '+' '-'
%left '*' '/'
%right UMINUS

%nonassoc IF THEN
%nonassoc ELSE

%token FUNC PRINT RETURN BREAK IF THEN ELSE WHILE FOR IN DO OPENBLOCK CLOSEBLOCK
%token VAR NUMBER IDENTIFIER STRING

%%
program :
      global_list {                                     // program -> global_list
        root = malloc (sizeof(node_t));                 // root = PROGRAM
        node_init (root, PROGRAM, NULL, 1, $1);         // $1 = GLOBAL_LIST
      }
    ;

global_list :
      global {                                          // global_list -> global
        $$ = malloc (sizeof(node_t));                   // $$ = GLOBAL_LIST
        node_init ($$, GLOBAL_LIST, NULL, 1, $1);       // $1 = GLOBAL
      }
    | global_list global {                              // global_list -> global_list global
        $$ = malloc (sizeof(node_t));                   // $$ = GLOBAL_LIST
        node_init ($$, GLOBAL_LIST, NULL, 2, $1, $2);   // $1 = GLOBAL_LIST, $2 = GLOBAL
      }
    ;

for_statement :
      FOR identifier IN expression '.' '.' expression DO statement {
          $$ = malloc (sizeof(node_t));                 // $$ = FOR_STATEMENT
          node_init ($$, FOR_STATEMENT, NULL, 4, $2, $4, $7, $9); // $2 = IDENTIFIER_DATA, etc.
      }
    ;

/*
    TODO:
    Include the remaining modified VSL grammar as specified in PS2 - starting with `global`

    HINT:
    Recall that 'node_init' takes any number of arguments where:
    1. Node to initialize.
    2. Node type (see "include/nodetypes.h").
    3. Data (so far, no nodes need any data, but consider e.g. the NUMBER_DATA or IDENTIFIER_DATA node types - what data should these contain?).
    4. Number of node children.
    5->. Children - Note that these are constructed by other semantic actions (e.g. `program` has one child that is constructed by the `global_list semantic action).
    This should be a pretty large file when you are done.

    HINT (OPTIONAL):
    Note that mallocing and initializing of nodes happens a lot.
    You may want to create C macros to reduce redundancy.
*/

%%

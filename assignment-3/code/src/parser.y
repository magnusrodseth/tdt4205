%{
#include <vslc.h>

/* State variables from the flex generated scanner */
extern int yylineno; // The line currently being read
extern char yytext[]; // The text of the last consumed lexeme

/* The main flex driver function used by the parser */
int yylex(void);

/* The function called by the parser when errors occur */
int yyerror(const char *error)
{
    fprintf(stderr, "%s on line %d\n", error, yylineno);
    exit(EXIT_FAILURE);
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
program:
    global_list {
        root = malloc(sizeof(node_t));
        node_init(root, PROGRAM, NULL, 1, $1);         // $1 = GLOBAL_LIST
    };

global_list:
    global {
        $$ = malloc(sizeof(node_t));
        node_init($$, GLOBAL_LIST, NULL, 1, $1);       // $1 = GLOBAL
    }
    | global_list global {
        $$ = malloc(sizeof(node_t));
        node_init($$, GLOBAL_LIST, NULL, 2, $1, $2);   // $1 = GLOBAL_LIST, $2 = GLOBAL
    };

global:
    function {
        $$ = malloc(sizeof(node_t));
        node_init($$, GLOBAL, NULL, 1, $1);            // $1 = FUNCTION
    }
    | declaration {
        $$ = malloc(sizeof(node_t));
        node_init($$, GLOBAL, NULL, 1, $1);            // $1 = DECLARATION
    }
    | array_declaration {
        $$ = malloc(sizeof(node_t));
        node_init($$, GLOBAL, NULL, 1, $1);            // $1 = ARRAY_DECLARATION
    };

declaration:
    VAR variable_list {
        $$ = malloc(sizeof(node_t));
        node_init($$, DECLARATION, NULL, 1, $2); // $2 = VARIABLE_LIST
    };

variable_list:
    identifier {
        $$ = malloc(sizeof(node_t));
        node_init($$, VARIABLE_LIST, NULL, 1, $1); // $1 = IDENTIFIER_DATA
    }
    | variable_list ',' identifier {
        $$ = malloc(sizeof(node_t));
        node_init($$, VARIABLE_LIST, NULL, 2, $1, $3); // $1 = VARIABLE_LIST, $3 = IDENTIFIER_DATA
    };

array_declaration:
    VAR array_indexing {
        $$ = malloc(sizeof(node_t));
        node_init($$, ARRAY_DECLARATION, NULL, 1, $2); // $2 = ARRAY_INDEXING
    };

array_indexing:
    identifier '[' expression ']' {
        $$ = malloc(sizeof(node_t));
        node_init($$, ARRAY_INDEXING, NULL, 2, $1, $3); // $1 = IDENTIFIER_DATA, $3 = EXPRESSION
    };

function:
    FUNC identifier '(' parameter_list ')' statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, FUNCTION, NULL, 3, $2, $4, $6); // $2 = IDENTIFIER_DATA, $4 = PARAMETER_LIST, $6 = STATEMENT
    };

parameter_list:
    variable_list {
        $$ = malloc(sizeof(node_t));
        node_init($$, PARAMETER_LIST, NULL, 1, $1); // $1 = VARIABLE_LIST
    }
    | %empty {
        $$ = malloc(sizeof(node_t));
        node_init($$, PARAMETER_LIST, NULL, 0);
    };

statement:
    assignment_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = ASSIGNMENT_STATEMENT
    }
    | print_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = PRINT_STATEMENT
    }
    | return_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = RETURN_STATEMENT
    }
    | break_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = BREAK_STATEMENT
    }
    | if_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = IF_STATEMENT
    }
    | while_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = WHILE_STATEMENT
    }
    | for_statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = FOR_STATEMENT
    }
    | block {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT, NULL, 1, $1); // $1 = BLOCK
    };

block:
    OPENBLOCK declaration_list statement_list CLOSEBLOCK {
        $$ = malloc(sizeof(node_t));
        node_init($$, BLOCK, NULL, 2, $2, $3); // $2 = DECLARATION_LIST, $3 = STATEMENT_LIST
    }
    | OPENBLOCK statement_list CLOSEBLOCK {
        $$ = malloc(sizeof(node_t));
        node_init($$, BLOCK, NULL, 1, $2); // $2 = STATEMENT_LIST
    };

declaration_list:
    declaration {
        $$ = malloc(sizeof(node_t));
        node_init($$, DECLARATION_LIST, NULL, 1, $1); // $1 = DECLARATION
    }
    | declaration_list declaration {
        $$ = malloc(sizeof(node_t));
        node_init($$, DECLARATION_LIST, NULL, 2, $1, $2); // $1 = DECLARATION_LIST, $2 = DECLARATION
    };

statement_list:
    statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT_LIST, NULL, 1, $1); // $1 = STATEMENT
    }
    | statement_list statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, STATEMENT_LIST, NULL, 2, $1, $2); // $1 = STATEMENT_LIST, $2 = STATEMENT
    };

assignment_statement:
    identifier ':' '=' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, ASSIGNMENT_STATEMENT, NULL, 2, $1, $4); // $1 = IDENTIFIER_DATA, $4 = EXPRESSION
    }
    | array_indexing ':' '=' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, ASSIGNMENT_STATEMENT, NULL, 2, $1, $4); // $1 = ARRAY_INDEXING, $4 = EXPRESSION
    };

return_statement:
    RETURN expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, RETURN_STATEMENT, NULL, 1, $2); // $2 = EXPRESSION
    }
    ;

print_statement:
    PRINT print_list {
        $$ = malloc(sizeof(node_t));
        node_init($$, PRINT_STATEMENT, NULL, 1, $2); // $2 = PRINT_LIST
    };

print_list:
    print_item {
        $$ = malloc(sizeof(node_t));
        node_init($$, PRINT_LIST, NULL, 1, $1); // $1 = PRINT_ITEM
    }
    | print_list ',' print_item {
        $$ = malloc(sizeof(node_t));
        node_init($$, PRINT_LIST, NULL, 2, $1, $3); // $1 = PRINT_LIST, $3 = PRINT_ITEM
    };

print_item:
    expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, PRINT_ITEM, NULL, 1, $1); // $1 = EXPRESSION
    }
    | string {
        $$ = malloc(sizeof(node_t));
        node_init($$, PRINT_ITEM, NULL, 1, $1); // $1 = STRING_DATA
    };

break_statement:
    BREAK {
        $$ = malloc(sizeof(node_t));
        node_init($$, BREAK_STATEMENT, NULL, 0);
    };

if_statement:
    IF relation THEN statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, IF_STATEMENT, NULL, 2, $2, $4); // $2 = RELATION, $4 = STATEMENT
    }
    | IF relation THEN statement ELSE statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, IF_STATEMENT, NULL, 3, $2, $4, $6); // $2 = RELATION, $4 = STATEMENT, $6 = STATEMENT
    };

while_statement:
    WHILE relation DO statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, WHILE_STATEMENT, NULL, 2, $2, $4); // $2 = RELATION, $4 = STATEMENT
    };

relation:
    expression '=' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, RELATION, strdup("="), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    }
    | expression '!' '=' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, RELATION, strdup("!="), 2, $1, $4); // $1 = EXPRESSION, $4 = EXPRESSION
    } 
    | expression '<' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, RELATION, strdup("<"), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    } 
    | expression '>' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, RELATION, strdup(">"), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    };

for_statement:
    FOR identifier IN expression '.' '.' expression DO statement {
        $$ = malloc(sizeof(node_t));
        node_init($$, FOR_STATEMENT, NULL, 4, $2, $4, $7, $9); // $2 = IDENTIFIER_DATA, $4 = EXPRESSION, $7 = EXPRESSION, $9 = STATEMENT
    };

expression:
    expression '+' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, strdup("+"), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    }
    | expression '-' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, strdup("-"), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    }
    | expression '*' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, strdup("*"), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    }
    | expression '/' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, strdup("/"), 2, $1, $3); // $1 = EXPRESSION, $3 = EXPRESSION
    }
    | '-' expression %prec UMINUS {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, strdup("-"), 1, $2); // $2 = EXPRESSION
    }
    | '(' expression ')' {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, NULL, 1, $2); // $2 = EXPRESSION
    }
    | number {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, NULL, 1, $1); // $1 = NUMBER
    }
    | identifier {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, NULL, 1, $1); // $1 = IDENTIFIER
    }
    | array_indexing {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, NULL, 1, $1); // $1 = ARRAY_INDEXING
    }
    | identifier '(' argument_list ')' {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION, strdup("call"), 2, $1, $3); // $1 = IDENTIFIER, $3 = ARGUMENT_LIST
    };

expression_list:
    expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION_LIST, NULL, 1, $1); // $1 = EXPRESSION
    }
    | expression_list ',' expression {
        $$ = malloc(sizeof(node_t));
        node_init($$, EXPRESSION_LIST, NULL, 2, $1, $3); // $1 = EXPRESSION_LIST, $3 = EXPRESSION
    };

argument_list:
    expression_list {
        $$ = malloc(sizeof(node_t));
        node_init($$, ARGUMENT_LIST, NULL, 1, $1); // $1 = EXPRESSION_LIST
    }
    | %empty {
        $$ = malloc(sizeof(node_t));
        node_init($$, ARGUMENT_LIST, NULL, 0);
    };

identifier:
    IDENTIFIER {
        $$ = malloc(sizeof(node_t));

        char* identifier = malloc(strlen(yytext));
        strncpy(identifier, yytext, strlen(yytext));
        identifier[strlen(yytext)] = '\0';

        node_init($$, IDENTIFIER_DATA, identifier, 0);
    };

number:
    NUMBER {
        $$ = malloc(sizeof(node_t));

        int64_t* number = malloc(sizeof(int64_t));
        *number = strtoll(yytext, NULL, 10);

        node_init($$, NUMBER_DATA, number, 0);
    };

string:
    STRING {
        $$ = malloc(sizeof(node_t));

        char* string = malloc(strlen(yytext));
        strncpy(string, yytext, strlen(yytext));
        string[strlen(yytext)] = '\0';

        node_init($$, STRING_DATA, string, 0);
    };
%%

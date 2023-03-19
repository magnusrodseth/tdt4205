#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <vslc.h>

/* Command line option parsing for the main function */
static void options ( int argc, char **argv );
static bool
    print_full_tree = false,
    print_simplified_tree = false,
    print_symbol_table_contents = false,
    print_generated_program = false;

/* Entry point */
int main ( int argc, char **argv )
{
    options ( argc, argv );

    yyparse ();       // Generated from grammar/bison, constructs syntax tree
    yylex_destroy (); // Free buffers used by flex

    // Operations in tree.c
    if ( print_full_tree )
        print_syntax_tree ();
    simplify_syntax_tree ();
    if ( print_simplified_tree )
        print_syntax_tree ();

    // Operations in symbols.c
     create_tables ();
    if ( print_symbol_table_contents )
        print_tables();

    // Operations in generator.c
    if ( print_generated_program )
        generate_program ();

    destroy_tables ();          // In symbols.c
    destroy_syntax_tree ();     // In tree.c
}

static const char *usage =
"Command line options\n"
"\t-h\tOutput this text and halt\n\n"
"\t-t\tOutput the full syntax tree\n"
"\t-T\tOutput the simplified syntax tree\n"
"\t-s\tOutput the symbol table contents\n"
"\t-c\tCompile and generate assembly output\n";


static void options ( int argc, char **argv )
{
    int o;
    while ( (o=getopt(argc,argv,"htTsc")) != -1 )
    {
        switch ( o )
        {
            case 'h':
                printf ( "%s:\n%s", argv[0], usage );
                exit ( EXIT_SUCCESS );
                break;
            case 't':   print_full_tree = true;             break;
            case 'T':   print_simplified_tree = true;       break;
            case 's':   print_symbol_table_contents = true; break;
            case 'c':   print_generated_program = true;     break;
        }
    }
}

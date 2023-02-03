%{
#include <vslc.h>
// The tokens defined in parser.y
#include "y.tab.h"
%}
%option noyywrap
%option array
%option yylineno

WHITESPACE [\ \t\v\r\n]
COMMENT \/\/[^\n]+
QUOTED \"([^\"\n]|\\\")*\"
FUNC func
BEGIN begin
END end
RETURN return
PRINT print
BREAK break
IF if
THEN then
ELSE else
FOR for
IN in
WHILE while
DO do
VAR var
NUMBER [0-9]+
IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*

%%
{WHITESPACE}+           { /* Eliminate whitespace */ }
{COMMENT}               { /* Eliminate comments */ }
{QUOTED}                { return STRING; }
{FUNC}                  { return FUNC; }
{BEGIN}                 { return OPENBLOCK; }
{END}                   { return CLOSEBLOCK; }
{RETURN}                { return RETURN; }
{PRINT}                 { return PRINT; }
{BREAK}                 { return BREAK; }
{IF}                    { return IF; }
{THEN}                  { return THEN; }
{ELSE}                  { return ELSE; }
{FOR}                   { return FOR; }
{IN}                    { return IN; }
{WHILE}                 { return WHILE; }
{DO}                    { return DO; }
{VAR}                   { return VAR; }
{NUMBER}                { return NUMBER; }
{IDENTIFIER}            { return IDENTIFIER; }
.                       { return yytext[0]; }
%%

int main(){
  int token = 0;

  while(token != EOF){
    token = yylex();

    switch(token){
      case FUNC:
        printf("FUNC");
        break;
      case OPENBLOCK:
        printf("OPENBLOCK");
        break;
      case CLOSEBLOCK:
        printf("CLOSEBLOCK");
        break;
      case RETURN:
        printf("RETURN");
        break;
      case PRINT:
        printf("PRINT");
        break;
      case BREAK:
        printf("BREAK");
        break;
      case IF:
        printf("IF");
        break;
      case THEN:
        printf("THEN");
        break;
      case ELSE:
        printf("ELSE");
        break;
      case FOR:
        printf("FOR");
        break;
      case IN:
        printf("IN");
        break;
      case WHILE:
        printf("WHILE");
        break;
      case DO:
        printf("DO");
        break;
      case VAR:
        printf("VAR");
        break;
      case NUMBER:
        printf("NUMBER");
        break;
      case IDENTIFIER:
        printf("IDENTIFIER");
        break;
      case STRING:
        printf("STRING");
        break;
      case EOF:
        printf("EOF");
        break;
      default:
        printf("DEFAULT");
        break;
}
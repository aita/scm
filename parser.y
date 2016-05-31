%{
#include "scheme.h"
#include <stdio.h>

#define YYSTYPE ScmObject*

extern char *yytext;
void yyerror(const char *s);
int yylex(void);
%}


%token LPAREN
%token RPAREN
%token DOT
%token QUOTE
%token STRING
%token TRUE
%token FALSE
%token INTEGER
%token SYMBOL

%%

toplevel:
    | expr toplevel
    {
        scheme_eval($1);
    }

expr: atom | pair

atom: SYMBOL
    {
        $$ = scheme_symbol(yytext);
    }
    | INTEGER
    {
        $$ = scheme_int(atoi(yytext));
    }
    | LPAREN RPAREN
    {
        $$ = SCM_NULL;
    }

pair: LPAREN list_elements RPAREN
    {
        $$ = $2;
    }

list_elements: expr
    {
        $$ = scheme_cons($1, SCM_NULL);
    }
    | expr DOT expr
    {
        $$ = scheme_cons($1, $3);
    }
    | expr list_elements
    {
        $$ = scheme_cons($1, $2);
    }

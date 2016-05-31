%{
#include <stdio.h>

void
yyerror(char *message)
{
    printf("Error: %s\n",message);
}

int
yywrap(void)
{
    return 1;
}
%}

%%

"+" { printf("PLUS\n"); }
"-" { printf("MINUS\n"); }

%%

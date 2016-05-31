%{
#include <stdio.h>
#include "parser.h"

void
yyerror(const char *message)
{
    printf("Error: %s\n", message);
}

int
yywrap(void)
{
    return 1;
}
%}

DIGIT                  [0-9]
LETTER                 [a-zA-Z]
SPECIAL_INITIAL        [!\$%&\*\/\:\<=\>\?\^\_~]
INITIAL                {LETTER}|{SPECIAL_INITIAL}|"-"
SUBSEQUENT             {INITIAL}|{DIGIT}|[\+\-\.@]
PECULIAR_IDENTIFIER    [\+\-]
IDENTIFIER             {INITIAL}{SUBSEQUENT}*|{PECULIAR_IDENTIFIER}

%%

"("                   { return LPAREN; }
")"                   { return RPAREN; }
"\."                  { return DOT; }
[\+\-]?{DIGIT}+       { return INTEGER; }
{IDENTIFIER}          { return SYMBOL; }

[ \t]+    { ; /* skip */ }
[\r\n]    { ; /* newline */ }

.         {
              char buf[1024];
              sprintf(buf, "invalid character: %s", yytext);
              yyerror(buf);
          }

%%

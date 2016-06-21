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
INLINE_WHITESPACE      [ \t]

SPECIAL_INITIAL        [!\$%&\*\/\:\<=\>\?\^\_~]
INITIAL                {LETTER}|{SPECIAL_INITIAL}|"-"
SUBSEQUENT             {INITIAL}|{DIGIT}|[\+\-\.@]
PECULIAR_IDENTIFIER    [\+\-]
IDENTIFIER             {INITIAL}{SUBSEQUENT}*|{PECULIAR_IDENTIFIER}
QUOTE                  \'
DOUBLE_QUOTE           \"
STRING                 {DOUBLE_QUOTE}({LETTER}|{INLINE_WHITESPACE})*{DOUBLE_QUOTE}

%%

"("                   { return LPAREN; }
")"                   { return RPAREN; }
"\."                  { return DOT; }
[\+\-]?{DIGIT}+       { return INTEGER; }
{IDENTIFIER}          { return SYMBOL; }
{STRING}              { return STRING; }

[ \t]+    { ; /* skip */ }
[\r\n]    { ; /* newline */ }

.         {
              char buf[1024];
              sprintf(buf, "invalid character: %s", yytext);
              yyerror(buf);
          }

%%

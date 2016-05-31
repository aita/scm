#include <stdio.h>

extern int yylex(void);
extern FILE *yyin;

int
main(int argc, char **argv)
{
    yyin = stdin;
    while (yylex()) {

    }
    return 0;
}

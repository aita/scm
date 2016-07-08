#include <stdio.h>
#include <string.h>
#include <scheme.h>
#include <parser.h>

extern FILE *yyin;

int
main(int argc, char **argv)
{
    if (!scheme_init())
        return 1;
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    int ret = yyparse();
    return ret;
}

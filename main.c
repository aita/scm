#include <stdio.h>
#include "parser.h"
#include "scheme.h"

int
main(int argc, char **argv)
{
    return yyparse();
}

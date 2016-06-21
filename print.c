#include "scheme.h"
#include <string.h>
#include <stdio.h>

static void scheme_print_int(ScmObject *o);
static void scheme_print_symbol(ScmObject *o);
static void scheme_print_string(ScmObject *o);
static void scheme_print_pair(ScmObject *o);

ScmObject *
scheme_print(ScmObject *args)
{
    ScmObject *obj = SCM_CAR(args);
    switch (SCM_TAG(obj)) {
    case SCM_TYPE_NULL:
        printf("()");
        break;
    case SCM_TYPE_INT:
        scheme_print_int(obj);
        break;
    case SCM_TYPE_SYMBOL:
        scheme_print_symbol(obj);
        break;
    case SCM_TYPE_STRING:
        scheme_print_string(obj);
        break;
    case SCM_TYPE_PAIR:
        scheme_print_pair(obj);
        break;
    default:
        printf("<unknown>");
    }
    return SCM_NULL;
}

static void
scheme_print_int(ScmObject *o)
{
    printf("%lu", ((ScmInt *)o)->val);
}

static void
scheme_print_symbol(ScmObject *o)
{
    ScmSymbol *sym = (ScmSymbol *)o;
    fwrite(sym->name->s, 1, sym->name->len, stdout);
}

static void
scheme_print_string(ScmObject *o)
{
    ScmString *str = (ScmString *)o;
    fputc('"', stdout);
    fwrite(str->s, 1, str->len, stdout);
    fputc('"', stdout);
}

static void
scheme_print_pair(ScmObject *o)
{
    ScmObject *next = o;
    printf("(");
    while (1) {
        ScmPair *pair = (ScmPair *)next;
        scheme_print(pair->car);
        next = pair->cdr;
        if (next == SCM_NULL) {
            break;
        } else if (SCM_TAG(next) != SCM_TYPE_PAIR) {
            printf(" . ");
            scheme_print(pair->cdr);
            break;
        } else {
            printf(" ");
        }
    }
    printf(")");
}

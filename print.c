#include <scheme.h>
#include <string.h>
#include <stdio.h>

static void scheme_print_int(ScmObject *o);
static void scheme_print_symbol(ScmObject *o);
static void scheme_print_string(ScmObject *o);
static void scheme_print_pair(ScmObject *o);

int
scheme_print_object(ScmObject *obj)
{
    switch (SCM_TYPE(obj)) {
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
    case SCM_TYPE_PROCEDURE:
        printf("<procedure: %x>", obj);
        break;
    case SCM_TYPE_SYNTAX:
        printf("<procedure: %x>", obj);
        break;
    default:
        printf("<unknown>");
    }
    return 0;
}

ScmObject *
scheme_print(ScmObject *args)
{
    if (SCM_CDR(args) != SCM_NULL) {
        fprintf(stderr, "Invalid number of arguments");
        return SCM_NULL;
    }
    scheme_print_object(SCM_CAR(args));
    puts("");
}

static void
scheme_print_int(ScmObject *o)
{
    printf("%ld", ((ScmInt *)o)->val);
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
        scheme_print_object(pair->car);
        next = pair->cdr;
        if (next == SCM_NULL) {
            break;
        } else if (SCM_TYPE(next) != SCM_TYPE_PAIR) {
            printf(" . ");
            scheme_print_object(pair->cdr);
            break;
        } else {
            printf(" ");
        }
    }
    printf(")");
}

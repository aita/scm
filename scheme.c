#include "scheme.h"
#include <string.h>
#include <stdio.h>

static void scheme_repr(ScmObject *o);
static void scheme_repr_int(ScmObject *o);
static void scheme_repr_symbol(ScmObject *o);
static void scheme_repr_pair(ScmObject *o);

ScmObject *
scheme_int(int i)
{
    ScmInt *obj = SCM_NEW_OBJECT(ScmInt);
    SCM_SET_TAG(obj, SCM_T_INT);
    obj->val = i;
    return (ScmObject *)obj;
}

ScmObject *
scheme_symbol(const char * s)
{
    ScmSymbol *obj = SCM_NEW_OBJECT(ScmSymbol);
    SCM_SET_TAG(obj, SCM_T_SYMBOL);
    obj->len = strlen(s);
    obj->s = scheme_malloc(obj->len);
    memcpy((void *)obj->s, (const void *)s, obj->len);
    return (ScmObject *)obj;
}

ScmObject *
scheme_cons(ScmObject *car, ScmObject *cdr)
{
    ScmPair *obj = SCM_NEW_OBJECT(ScmPair);
    SCM_SET_TAG(obj, SCM_T_PAIR);
    obj->car = car;
    obj->cdr = cdr;
    return (ScmObject *)obj;
}

int
scheme_eval(ScmObject *obj)
{
    scheme_repr(obj);
    printf("\n");
    return 0;
}

void
scheme_repr(ScmObject *obj)
{
    switch (SCM_TAG(obj)) {
    case SCM_T_NULL:
        printf("()");
        break;
    case SCM_T_INT:
        scheme_repr_int(obj);
        break;
    case SCM_T_SYMBOL:
        scheme_repr_symbol(obj);
        break;
    case SCM_T_PAIR:
        scheme_repr_pair(obj);
        break;
    default:
        printf("<unknown>");
    }
}

static void
scheme_repr_int(ScmObject *o)
{
    printf("%d", ((ScmInt *)o)->val);
}

static void
scheme_repr_symbol(ScmObject *o)
{
    ScmSymbol *sym = (ScmSymbol *)o;
    fwrite(sym->s, 1, sym->len, stdout);
}

static void
scheme_repr_pair(ScmObject *o)
{
    ScmObject *next = o;
    printf("(");
    while (1) {
        ScmPair *pair = (ScmPair *)next;
        scheme_repr(pair->car);
        next = pair->cdr;
        if (next == SCM_NULL) {
            break;
        } else if (SCM_TAG(next) != SCM_T_PAIR) {
            printf(" . ");
            scheme_repr(pair->cdr);
            break;
        } else {
            printf(" ");
        }
    }
    printf(")");
}

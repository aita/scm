#include "scheme.h"
#include <string.h>
#include <stdio.h>

SCM *scheme;

int
scheme_init()
{
    scheme = scheme_malloc(sizeof(SCM));
    if (scheme == NULL)
        return 0;
    scheme->env = SCM_NULL;
    scheme->symbols = SCM_NULL;

    // register default procedures
    scheme_register(scheme_symbol("print"), scheme_procedure(scheme_print));

    return 1;
}

ScmObject *
find_symbol(const char *name)
{
    ScmObject *o;
    for (o = scheme->symbols; o != SCM_NULL; o = SCM_CDR(o)) {
        ScmObject *symbol = SCM_CAR(o);
        if (!scheme_strcmp(SCM_SYMBOL_NAME(symbol), name))
            return symbol;
    }
    return SCM_NULL;
}

void
scheme_register(ScmObject *symbol, ScmObject *obj)
{
    ScmObject *p;
    ScmObject *bound = SCM_NULL;
    for (p = scheme->env; p != SCM_NULL; p = SCM_CDR(p)) {
        if ((bound = SCM_CAR(p)) == symbol)
            break;
    }
    if (bound != SCM_NULL) {
        SCM_CDR(bound) = obj;
    } else {
        scheme->env = scheme_cons(scheme_cons(symbol, obj), scheme->env);
    }
}

static ScmObject *
lookup(ScmObject *symbol)
{
    ScmObject *p;
    for (p = scheme->env; p != SCM_NULL; p = SCM_CDR(p)) {
        ScmObject *pair = SCM_CAR(p);
        if (SCM_CAR(pair) == symbol)
            return SCM_CDR(pair);
    }
    return SCM_NULL;
}

int
scheme_apply(ScmObject *obj)
{
    ScmSymbol *symbol = (ScmSymbol *)SCM_CAR(obj);
    if (SCM_TAG(symbol) != SCM_TYPE_SYMBOL) {
        fprintf(stderr, "Wrong type to apply.\n");
        return 0;
    }
    ScmObject *var = lookup((ScmObject *)symbol);
    if (SCM_TAG(var) != SCM_TYPE_PROCEDURE) {
        fprintf(stderr, "Wrong type to apply.\n");
        return 0;
    }
    if (var == SCM_NULL) {
        fprintf(stderr, "Unbound variable: ");
        fwrite(symbol->name->s, 1, symbol->name->len, stderr);
        fprintf(stderr, "\n");
        return 0;
    }
    ScmProcedure *proc = (ScmProcedure *)var;
    proc->fn(SCM_CDR(obj));
    return 1;
}

int
scheme_eval(ScmObject *obj)
{
    switch (SCM_TAG(obj)) {
    case SCM_TYPE_PAIR:
        scheme_apply(obj);
        break;
    default:
        scheme_print(obj);
        printf("\n");
    }
    return 0;
}

ScmObject *
scheme_int(int i)
{
    ScmInt *obj = SCM_NEW_OBJECT(ScmInt);
    SCM_SET_TAG(obj, SCM_TYPE_INT);
    obj->val = i;
    return (ScmObject *)obj;
}

ScmObject *
scheme_symbol(const char *name)
{
    ScmSymbol *obj;

    if ((obj = (ScmSymbol *)find_symbol(name)) != SCM_NULL)
        return (ScmObject *)obj;

    obj = SCM_NEW_OBJECT(ScmSymbol);
    SCM_SET_TAG(obj, SCM_TYPE_SYMBOL);
    obj->name = (ScmString *)scheme_string(name);
    scheme->symbols = scheme_cons((ScmObject *)obj, scheme->symbols);
    return (ScmObject *)obj;
}

ScmObject *
scheme_cons(ScmObject *car, ScmObject *cdr)
{
    ScmPair *obj = SCM_NEW_OBJECT(ScmPair);
    SCM_SET_TAG(obj, SCM_TYPE_PAIR);
    obj->car = car;
    obj->cdr = cdr;
    return (ScmObject *)obj;
}

ScmObject *
scheme_string(const char *s)
{
    ScmString *obj = SCM_NEW_OBJECT(ScmString);
    SCM_SET_TAG(obj, SCM_TYPE_STRING);
    obj->len = strlen(s);
    obj->s = scheme_malloc(obj->len);
    memcpy((void *)obj->s, (const void *)s, obj->len);
    return (ScmObject *)obj;
}

ScmObject *
scheme_procedure(scheme_function_t fn)
{
    ScmProcedure *obj = SCM_NEW_OBJECT(ScmProcedure);
    SCM_SET_TAG(obj, SCM_TYPE_PROCEDURE);
    obj->fn = fn;
    return (ScmObject *)obj;
}

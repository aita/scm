#include <scheme.h>
#include <string.h>
#include <stdio.h>

SCM *scheme;

static ScmObject *find_symbol(const char *name);
static ScmObject *lookup(ScmObject *obj);
static ScmObject *scheme_eval_symbol(ScmObject *obj);
static ScmObject *scheme_eval_pair(ScmObject *obj);
static ScmObject *scheme_eval_syntax(ScmObject *syntax, ScmObject *expr);
static ScmObject *scheme_eval_arguments(ScmObject *args);
static ScmObject *scheme_apply_procedure(ScmObject *proc, ScmObject *args);

int
scheme_init()
{
    scheme = scheme_malloc(sizeof(SCM));
    if (scheme == NULL)
        return 0;
    scheme->env = SCM_NULL;
    scheme->symbols = SCM_NULL;

    // register default procedures
    scheme_register("if", scheme_syntax(scheme_if));
    scheme_register("print", scheme_procedure(scheme_print));
    scheme_register("+", scheme_procedure(scheme_plus));
    scheme_register("-", scheme_procedure(scheme_minus));
    return 1;
}

static ScmObject *
find_symbol(const char *name)
{
    ScmObject *o;
    ScmObject *ret = SCM_NULL;
    for (o = scheme->symbols; o != SCM_NULL; o = SCM_CDR(o)) {
        ScmObject *symbol = SCM_CAR(o);
        if (scheme_strcmp(SCM_SYMBOL_NAME(symbol), name) == 0) {
            ret = symbol;
            break;
        }
    }
    return ret;
}

void
scheme_register(const char *name, ScmObject *obj)
{
    ScmObject *p;
    ScmObject *bound = SCM_NULL;
    ScmObject *symbol = scheme_symbol(name);
    for (p = scheme->env; p != SCM_NULL; p = SCM_CDR(p)) {
        if (SCM_CAR(p) == symbol) {
            bound = SCM_CAR(p);
            break;
        }
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

ScmObject *
scheme_apply(ScmObject *proc, ScmObject *args)
{
    switch (SCM_TYPE(proc)) {
    case SCM_TYPE_PROCEDURE:
        if (args != SCM_NULL) {
            args = scheme_eval_arguments(args);
            if (args == SCM_NULL)
                return SCM_NULL;
        }
        return scheme_apply_procedure(proc, args);
    default:
        fprintf(stderr, "Wrong type to apply.\n");
        return SCM_NULL;
    }
}
static ScmObject *
scheme_apply_procedure(ScmObject *proc, ScmObject *args)
{
    return ((ScmProcedure *)proc)->fn(args);
}


static ScmObject *
scheme_eval_arguments(ScmObject *args)
{
    ScmObject *ret = scheme_cons(SCM_NULL, SCM_NULL);
    ScmObject *cur = ret;
    for(;;) {
        SCM_CAR(cur) = scheme_eval(SCM_CAR(args));
        // check type of cdr is a pair or null
        args = SCM_CDR(args);
        scheme_tag_t tag = SCM_TYPE(args);
        if (tag != SCM_TYPE_PAIR && tag != SCM_TYPE_NULL) {
            fprintf(stderr, "syntax error");
            return SCM_NULL;
        }
        if (args == SCM_NULL) {
            break;
        }
        SCM_CDR(cur) = scheme_cons(SCM_NULL, SCM_NULL);
        cur = SCM_CDR(cur);
    }
    return ret;
}

ScmObject *
scheme_eval(ScmObject *obj)
{
    switch (SCM_TYPE(obj)) {
    case SCM_TYPE_SYMBOL:
        return scheme_eval_symbol(obj);
    case SCM_TYPE_PAIR:
        return scheme_eval_pair(obj);
    }
    return obj;
}

static ScmObject *
scheme_eval_symbol(ScmObject *obj)
{
    ScmObject *var = lookup(obj);
    if (var == SCM_NULL) {
        ScmSymbol *symbol = (ScmSymbol *)obj;
        fprintf(stderr, "Unbound variable: ");
        fwrite(symbol->name->s, 1, symbol->name->len, stderr);
        fprintf(stderr, "\n");
        return SCM_NULL;
    }
    return var;
}

static ScmObject *
scheme_eval_pair(ScmObject *obj)
{
    ScmObject *car = SCM_CAR(obj);
    if (SCM_TYPE(car) == SCM_TYPE_SYMBOL) {
        ScmObject *var = scheme_eval_symbol(car);
        if (var == SCM_NULL) {
            return SCM_NULL;
        }
        car = var;
    }
    switch (SCM_TYPE(car))  {
    case SCM_TYPE_SYNTAX:
        return scheme_eval_syntax(car, SCM_CDR(obj));
    case SCM_TYPE_PROCEDURE:
    case SCM_TYPE_CLOSURE:
        return scheme_apply(car, SCM_CDR(obj));
    }
}

ScmObject *
scheme_eval_syntax(ScmObject *syntax, ScmObject *expr)
{
    return ((ScmSyntax *)syntax)->fn(expr);
}

void
scheme_error(const char *message)
{
    fputs(message, stderr);
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
    ScmSymbol *obj = (ScmSymbol *)find_symbol(name);
    if (obj != SCM_NULL) {
        return (ScmObject *)obj;
    }
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

ScmObject *
scheme_syntax(scheme_function_t fn)
{
    ScmSyntax *obj = SCM_NEW_OBJECT(ScmSyntax);
    SCM_SET_TAG(obj, SCM_TYPE_SYNTAX);
    obj->fn = fn;
    return (ScmObject *)obj;
}

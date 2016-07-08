#include <scheme.h>
#include <string.h>
#include <stdio.h>

SCM *scheme;

// syntax functions
ScmObject *scheme_syntax_if(ScmObject *expr);
ScmObject *scheme_syntax_define(ScmObject *expr);

// static functions
static ScmObject *find_symbol(const char *name);
static ScmObject *lookup(ScmObject *obj);
static ScmObject *lookup_cell(ScmObject *obj);
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
    scheme_register("if", scheme_syntax(scheme_syntax_if));
    scheme_register("define", scheme_syntax(scheme_syntax_define));
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

static ScmObject *
lookup(ScmObject *symbol)
{
    ScmObject *cell = lookup_cell(symbol);
    if (cell != SCM_NULL)
        return SCM_CDR(cell);
    return SCM_NULL;
}

static ScmObject *
lookup_cell(ScmObject *symbol)
{
    ScmObject *p;
    for (p = scheme->env; p != SCM_NULL; p = SCM_CDR(p)) {
        ScmObject *cell = SCM_CAR(p);
        if (SCM_CAR(cell) == symbol)
            return cell;
    }
    return SCM_NULL;
}

void
scheme_define(ScmObject *symbol, ScmObject *value)
{
    ScmObject *cell = lookup_cell(symbol);
    if (cell != SCM_NULL) {
        SCM_CDR(cell) = value;
    } else {
        scheme->env = scheme_cons(scheme_cons(symbol, value), scheme->env);
    }
}

void
scheme_register(const char *name, ScmObject *obj)
{
    ScmObject *symbol = scheme_symbol(name);
    scheme_define(symbol, obj);
}

ScmObject *
scheme_apply(ScmObject *proc, ScmObject *args)
{
    switch (SCM_TYPE(proc)) {
    case SCM_TYPE_PROCEDURE:
        if (args != SCM_NULL) {
            args = scheme_eval_arguments(args);
            if (SCM_ERRORP(args))
                return args;
        }
        return scheme_apply_procedure(proc, args);
    default:
        return scheme_error("Wrong type to apply.");
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
            return scheme_error("syntax error");
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
    ScmObject *ret = SCM_NULL;
    switch (SCM_TYPE(obj)) {
    case SCM_TYPE_SYMBOL:
        ret = scheme_eval_symbol(obj);
        break;
    case SCM_TYPE_PAIR:
        ret = scheme_eval_pair(obj);
        break;
    default:
        ret = obj;
    }
    if (SCM_ERRORP(ret)) {
        ScmString *message = (ScmString *)SCM_ERROR_MESSAGE(ret);
        fwrite(message->s, 1, message->len, stderr);
        fprintf(stderr, "\n");
    }
    return ret;
}

static ScmObject *
scheme_eval_symbol(ScmObject *obj)
{
    ScmObject *var = lookup(obj);
    if (var == SCM_NULL) {
        ScmSymbol *symbol = (ScmSymbol *)obj;
        /*
        fprintf(stderr, "Unbound variable: ");
        fwrite(symbol->name->s, 1, symbol->name->len, stderr);
        fprintf(stderr, "\n");
        return SCM_NULL;
        */
        return scheme_error("unbound variable");
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
    default:
        return scheme_error("syntax error");
    }
}

ScmObject *
scheme_eval_syntax(ScmObject *syntax, ScmObject *expr)
{
    return ((ScmSyntax *)syntax)->fn(expr);
}

ScmObject *
scheme_error(const char *message)
{
    ScmError *obj = SCM_NEW_OBJECT(ScmError);
    SCM_SET_TAG(obj, SCM_TYPE_ERROR);
    obj->message = (ScmString *)scheme_string(message);
    return (ScmObject *)obj;
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

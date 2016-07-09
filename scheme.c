#include <scheme.h>
#include <string.h>
#include <stdio.h>

SCM *scheme;

// syntax functions
ScmObject *scheme_syntax_if(ScmObject *expr, ScmObject *env);
ScmObject *scheme_syntax_define(ScmObject *expr, ScmObject *env);

// static functions
static ScmObject *find_symbol(const char *name);
static ScmObject *lookup(ScmObject *obj, ScmObject *env);
static ScmObject *lookup_cell(ScmObject *obj, ScmObject *env);
static ScmObject *scheme_eval_symbol(ScmObject *obj, ScmObject *env);
static ScmObject *scheme_eval_pair(ScmObject *obj, ScmObject *env);
static ScmObject *scheme_eval_sequence(ScmObject *obj, ScmObject *env);
static ScmObject *scheme_eval_syntax(ScmObject *syntax, ScmObject *expr, ScmObject *env);
static ScmObject *scheme_eval_arguments(ScmObject *args, ScmObject *env);
static ScmObject *scheme_apply_procedure(ScmObject *obj, ScmObject *args);
static ScmObject *scheme_apply_closure(ScmObject *obj, ScmObject *args, ScmObject *env);

int
scheme_init()
{
    scheme = scheme_malloc(sizeof(SCM));
    if (scheme == NULL)
        return 0;
    scheme->env = scheme_cons(SCM_NULL, SCM_NULL);
    scheme->symbols = SCM_NULL;

    // register syntax and default procedures
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
    for (o = scheme->symbols; !SCM_NULLP(o); o = SCM_CDR(o)) {
        ScmObject *symbol = SCM_CAR(o);
        if (scheme_strcmp(SCM_SYMBOL_NAME(symbol), name) == 0) {
            ret = symbol;
            break;
        }
    }
    return ret;
}

static ScmObject *
lookup(ScmObject *symbol, ScmObject *env)
{
    ScmObject *cell = lookup_cell(symbol, env);
    if (cell != SCM_NULL)
        return SCM_CDR(cell);
    return SCM_NULL;
}

static ScmObject *
lookup_cell(ScmObject *symbol, ScmObject *env)
{
    ScmObject *frame;
    for (; !SCM_NULLP(env); env = SCM_CDR(env)) {
        ScmObject *frame = SCM_CAR(env);
        for (; !SCM_NULLP(frame); frame = SCM_CDR(frame)) {
            ScmObject *cell = SCM_CAR(frame);
            if (SCM_CAR(cell) == symbol)
                return cell;
        }
    }
    return SCM_NULL;
}

void
scheme_define(ScmObject *symbol, ScmObject *value, ScmObject *env)
{
    ScmObject *cell = lookup_cell(symbol, env);
    if (!SCM_NULLP(cell)) {
        SCM_CDR(cell) = value;
    } else {
        SCM_CAR(env) = scheme_cons(scheme_cons(symbol, value), SCM_CAR(env));
    }
}

void
scheme_register(const char *name, ScmObject *obj)
{
    ScmObject *symbol = scheme_symbol(name);
    scheme_define(symbol, obj, scheme->env);
}

ScmObject *
scheme_apply(ScmObject *obj, ScmObject *args, ScmObject *env)
{
    switch (SCM_TYPE(obj)) {
    case SCM_TYPE_PROCEDURE:
        if (!SCM_NULLP(args)) {
            args = scheme_eval_arguments(args, env);
            if (SCM_ERRORP(args))
                return args;
        }
        return scheme_apply_procedure(obj, args);
    case SCM_TYPE_CLOSURE:
        if (!SCM_NULLP(args)) {
            args = scheme_eval_arguments(args, env);
            if (SCM_ERRORP(args))
                return args;
        }
        return scheme_apply_closure(obj, args, env);
    default:
        return scheme_error("Wrong type to apply.");
    }
}

static ScmObject *
scheme_apply_procedure(ScmObject *obj, ScmObject *args)
{
    return ((ScmProcedure *)obj)->fn(args);
}

static ScmObject *
scheme_apply_closure(ScmObject *obj, ScmObject *args, ScmObject *env)
{
    ScmClosure *closure = (ScmClosure *)obj;
    ScmObject *params = closure->params;
    ScmObject *frame = SCM_NULL;
    for (; !SCM_NULLP(params) || !SCM_NULLP(args); params = SCM_CDR(params), args = SCM_CDR(args)) {
        ScmObject *symbol = SCM_CAR(params);
        ScmObject *value = scheme_eval(SCM_CAR(args), env);
        ScmObject *cell = scheme_cons(symbol, value);
        frame = scheme_cons(cell, frame);
    }
    return scheme_eval_sequence(closure->body, scheme_cons(frame, env));
}

static ScmObject *
scheme_eval_arguments(ScmObject *args, ScmObject *env)
{
    ScmObject *ret = scheme_cons(SCM_NULL, SCM_NULL);
    ScmObject *cur = ret;
    for (;;) {
        SCM_CAR(cur) = scheme_eval(SCM_CAR(args), env);
        args = SCM_CDR(args);
        if (SCM_NULLP(args)) {
            break;
        }
        if (!SCM_PAIRP(args)) {
            return scheme_error("syntax error");
        }
        SCM_CDR(cur) = scheme_cons(SCM_NULL, SCM_NULL);
        cur = SCM_CDR(cur);
    }
    return ret;
}

ScmObject *
scheme_eval(ScmObject *obj, ScmObject *env)
{
    ScmObject *ret = SCM_NULL;
    switch (SCM_TYPE(obj)) {
    case SCM_TYPE_SYMBOL:
        ret = scheme_eval_symbol(obj, env);
        break;
    case SCM_TYPE_PAIR:
        ret = scheme_eval_pair(obj, env);
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
scheme_eval_symbol(ScmObject *obj, ScmObject *env)
{
    ScmObject *var = lookup(obj, env);
    if (SCM_NULLP(var)) {
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
scheme_eval_pair(ScmObject *obj, ScmObject *env)
{
    ScmObject *car = SCM_CAR(obj);
    if (SCM_SYMBOLP(car)) {
        ScmObject *var = scheme_eval_symbol(car, env);
        if (SCM_NULLP(var)) {
            return SCM_NULL;
        }
        car = var;
    }
    switch (SCM_TYPE(car))  {
    case SCM_TYPE_SYNTAX:
        return scheme_eval_syntax(car, SCM_CDR(obj), env);
    case SCM_TYPE_PROCEDURE:
    case SCM_TYPE_CLOSURE:
        return scheme_apply(car, SCM_CDR(obj), env);
    default:
        return scheme_error("syntax error");
    }
}

static ScmObject *
scheme_eval_sequence(ScmObject *seq, ScmObject *env)
{
    ScmObject *ret;
    for (; !SCM_NULLP(seq); seq = SCM_CDR(seq)){
        ret = scheme_eval(SCM_CAR(seq), env);
    }
    return ret;
}


ScmObject *
scheme_eval_syntax(ScmObject *syntax, ScmObject *expr, ScmObject *env)
{
    return ((ScmSyntax *)syntax)->fn(expr, env);
}

ScmObject *
scheme_error(const char *message)
{
    ScmError *obj = SCM_NEW(ScmError);
    SCM_SET_TAG(obj, SCM_TYPE_ERROR);
    obj->message = (ScmString *)scheme_string(message);
    return (ScmObject *)obj;
}

ScmObject *
scheme_int(int i)
{
    ScmInt *obj = SCM_NEW(ScmInt);
    SCM_SET_TAG(obj, SCM_TYPE_INT);
    obj->val = i;
    return (ScmObject *)obj;
}

ScmObject *
scheme_symbol(const char *name)
{
    ScmSymbol *obj = (ScmSymbol *)find_symbol(name);
    if (!SCM_NULLP(obj)) {
        return (ScmObject *)obj;
    }
    obj = SCM_NEW(ScmSymbol);
    SCM_SET_TAG(obj, SCM_TYPE_SYMBOL);
    obj->name = (ScmString *)scheme_string(name);
    scheme->symbols = scheme_cons((ScmObject *)obj, scheme->symbols);
    return (ScmObject *)obj;
}

ScmObject *
scheme_cons(ScmObject *car, ScmObject *cdr)
{
    ScmPair *obj = SCM_NEW(ScmPair);
    SCM_SET_TAG(obj, SCM_TYPE_PAIR);
    obj->car = car;
    obj->cdr = cdr;
    return (ScmObject *)obj;
}

ScmObject *
scheme_string(const char *s)
{
    ScmString *obj = SCM_NEW(ScmString);
    SCM_SET_TAG(obj, SCM_TYPE_STRING);
    obj->len = strlen(s);
    obj->s = scheme_malloc(obj->len);
    memcpy((void *)obj->s, (const void *)s, obj->len);
    return (ScmObject *)obj;
}

ScmObject *
scheme_syntax(scheme_syntax_t fn)
{
    ScmSyntax *obj = SCM_NEW(ScmSyntax);
    SCM_SET_TAG(obj, SCM_TYPE_SYNTAX);
    obj->fn = fn;
    return (ScmObject *)obj;
}

ScmObject *
scheme_procedure(scheme_function_t fn)
{
    ScmProcedure *obj = SCM_NEW(ScmProcedure);
    SCM_SET_TAG(obj, SCM_TYPE_PROCEDURE);
    obj->fn = fn;
    return (ScmObject *)obj;
}

ScmObject *
scheme_closure(ScmObject *params, ScmObject *body)
{
    ScmClosure *obj = SCM_NEW(ScmClosure);
    SCM_SET_TAG(obj, SCM_TYPE_CLOSURE);
    obj->params = params;
    obj->body = body;
    return (ScmObject *)obj;
}

#include <scheme.h>

ScmObject *
scheme_syntax_if(ScmObject *expr, ScmObject *env)
{
    if (SCM_NULLP(expr))
        goto error;
    if (SCM_NULLP(SCM_CDR(expr)))
        goto error;
    if (SCM_NULLP(SCM_CDDR(expr)))
        goto error;
    if (!SCM_NULLP(SCM_CDDDR(expr)))
        goto error;

    ScmObject *cond_expr = SCM_CAR(expr);
    ScmObject *then_expr = SCM_CADR(expr);
    ScmObject *else_expr = SCM_CADDR(expr);

    ScmObject *cond = scheme_eval(cond_expr, env);
    if (cond != SCM_NULL) {
        return scheme_eval(then_expr, env);
    } else {
        return scheme_eval(else_expr, env);
    }

error:
    scheme_error("syntax error");
    return SCM_NULL;
}

ScmObject *
scheme_syntax_define(ScmObject *expr, ScmObject *env)
{
    if (SCM_NULLP(expr))
        goto error;

    ScmObject *symbol, *value;
    if (SCM_SYMBOLP(SCM_CAR(expr))) {
        symbol = SCM_CAR(expr);
        value = scheme_eval(SCM_CADR(expr), env);
    } else if (SCM_PAIRP(SCM_CAR(expr))) {
        symbol = SCM_CAAR(expr);
        value = scheme_closure(SCM_CDAR(expr), SCM_CDR(expr));
    } else {
        goto error;
    }
    if (SCM_ERRORP(value))
        return value;
    scheme_define(symbol, value, env);
    return value;

error:
    scheme_error("syntax error");
    return SCM_NULL;
}

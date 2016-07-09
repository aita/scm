#include <scheme.h>

ScmObject *
scheme_syntax_if(ScmObject *expr)
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

    ScmObject *cond = scheme_eval(cond_expr);
    if (cond != SCM_NULL) {
        return scheme_eval(then_expr);
    } else {
        return scheme_eval(else_expr);
    }

error:
    return scheme_error("syntax error");
;
}

ScmObject *
scheme_syntax_define(ScmObject *expr)
{
    if (SCM_NULLP(expr))
        goto error;
    if (SCM_NULLP(SCM_CDR(expr)))
        goto error;
    if (!SCM_NULLP(SCM_CDDR(expr)))
        goto error;

    ScmObject *symbol = SCM_CAR(expr);
    ScmObject *value = scheme_eval(SCM_CADR(expr));
    if (SCM_ERRORP(value))
        return value;
    scheme_define(symbol, value);
    return value;

error:
    return scheme_error("syntax error");
}

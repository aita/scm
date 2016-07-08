#include <scheme.h>

ScmObject *
scheme_if(ScmObject *expr)
{
    if (expr == SCM_NULL)
        goto error;
    if (SCM_CDR(expr) == SCM_NULL)
        goto error;
    if (SCM_CDDR(expr) == SCM_NULL)
        goto error;
    if (SCM_CDDDR(expr) != SCM_NULL)
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
    scheme_error("syntax error");
    return SCM_NULL;
}

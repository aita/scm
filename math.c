#include <scheme.h>

ScmObject *
scheme_plus(ScmObject *args)
{
    ScmObject *ret = scheme_int(0);
    ScmObject *next;
    for (next = args; !SCM_NULLP(next); next = SCM_CDR(next)) {
        ScmObject *obj = SCM_CAR(next);
        if (!SCM_INTP(obj)) {
            return scheme_error("Wrong type of argument");
        }
        SCM_INT(ret) += SCM_INT(obj);
    }
    return ret;
}

ScmObject *
scheme_minus(ScmObject *args)
{
    if (args == SCM_NULL)
        return scheme_int(0);

    ScmObject *next;
    ScmObject *ret = SCM_CAR(args);
    if (!SCM_INTP(ret)) {
        return scheme_error("Wrong type of argument");
    }
    for (next = SCM_CDR(args); !SCM_NULLP(next); next = SCM_CDR(next)) {
        ScmObject *obj = SCM_CAR(next);
        if (!SCM_INTP(obj)) {
            return scheme_error("Wrong type of argument");
        }
        SCM_INT(ret) -= SCM_INT(obj);
    }
    return ret;
}

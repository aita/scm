#include <scheme.h>

ScmObject *
scheme_plus(ScmObject *args)
{
    ScmObject *ret = scheme_int(0);
    ScmObject *next;
    for (next = args; next != SCM_NULL; next = SCM_CDR(next)) {
        ScmObject *obj = SCM_CAR(next);
        if (SCM_TYPE(obj) != SCM_TYPE_INT) {
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
        return 0;

    ScmObject *next;
    ScmObject *ret = SCM_CAR(args);
    if (SCM_TYPE(ret) != SCM_TYPE_INT) {
        return SCM_NULL;
    }
    for (next = SCM_CDR(args); next != SCM_NULL; next = SCM_CDR(next)) {
        ScmObject *obj = SCM_CAR(next);
        if (SCM_TYPE(obj) != SCM_TYPE_INT) {
            return scheme_error("Wrong type of argument");
        }
        SCM_INT(ret) -= SCM_INT(obj);
    }
    return ret;
}

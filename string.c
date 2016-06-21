#include "scheme.h"
#include <string.h>

int
scheme_strcmp(const ScmObject *obj, const char *s)
{
    int i;
    ScmString *strobj = (ScmString *)obj;
    const scheme_char_t *s1 = s;
    const scheme_char_t *s2 = strobj->s;
    for (i = 0; *s1 == *s2 && i < strobj->len+1; i++, s1++, s2++) {
        if (*s1 == '\0')
            return 0;
    }
    return ((*s1 < *s2) ? -1 : +1);
}

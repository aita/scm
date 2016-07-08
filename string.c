#include "scheme.h"
#include <string.h>

int
scheme_strcmp(const ScmObject *obj, const char *s)
{
    int i;
    ScmString *str = (ScmString *)obj;
    const scheme_char_t *s1 = s;
    const scheme_char_t *s2 = str->s;
    for (i = 0; *s1 != '\0' && i < str->len+1; i++, s1++, s2++) {
        if (*s1 != *s2) {
            return ((*s1 < *s2) ? -1 : +1);
        }
    }
    return 0;
}

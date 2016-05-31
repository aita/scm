#ifndef _SCHEME_H_
#define _SCHEME_H_
#include <stdlib.h>

#define scheme_malloc malloc
#define scheme_free free
#define SCM_NULL NULL

#define SCM_OBJECT_HEADER unsigned int tag;
#define SCM_TAG(OBJ) ((OBJ != SCM_NULL) ? OBJ->tag : SCM_T_NULL)

#define SCM_NEW_OBJECT(SCM_TYPE) \
    (SCM_TYPE *)scheme_malloc(sizeof(SCM_TYPE))

#define SCM_SET_TAG(OBJ, TAG) \
    do { \
        OBJ->tag = TAG; \
    } while(0)

typedef int scheme_int_t;
typedef unsigned char scheme_char_t;

struct ScmObject;
typedef struct ScmObject ScmObject;

#define SCM_T_NULL   0
#define SCM_T_INT    1
#define SCM_T_PAIR   2
#define SCM_T_SYMBOL 3
#define SCM_T_STRING 4
#define SCM_T_PORT   5


typedef struct ScmObject {
    SCM_OBJECT_HEADER
} ScmObject;

typedef struct ScmInt {
    SCM_OBJECT_HEADER
    scheme_int_t val;
} ScmInt;

typedef struct ScmSymbol {
    SCM_OBJECT_HEADER
    size_t len;
    const scheme_char_t *s;
} ScmSymbol;

typedef struct ScmString {
    SCM_OBJECT_HEADER
    size_t len;
    const scheme_char_t *s;
} ScmString;

typedef struct ScmPair {
    SCM_OBJECT_HEADER
    ScmObject *car;
    ScmObject *cdr;
} ScmPair;


int scheme_eval(ScmObject *obj);
ScmObject * scheme_int(int i);
ScmObject * scheme_symbol(const char * s);
ScmObject * scheme_cons(ScmObject *car, ScmObject *cdr);

#endif  /* _SCHEME_H_ */

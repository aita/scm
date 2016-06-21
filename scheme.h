#ifndef _SCHEME_H_
#define _SCHEME_H_
#include <stdlib.h>

#define scheme_malloc malloc
#define scheme_free free
#define SCM_NULL NULL

#define SCM_OBJECT_HEADER unsigned int tag;
#define SCM_TAG(OBJ) ((OBJ != SCM_NULL) ? OBJ->tag : SCM_TYPE_NULL)

#define SCM_NEW_OBJECT(SCM_TYPE) \
    (SCM_TYPE *)scheme_malloc(sizeof(SCM_TYPE))

#define SCM_SET_TAG(OBJ, TAG) \
    do { \
        OBJ->tag = TAG; \
    } while(0)

#define SCM_INT(obj) (obj->val)
#define SCM_SYMBOL_NAME(obj) (ScmObject *)(((ScmSymbol *)obj)->name)

#define SCM_CAR(obj) (((ScmPair *)obj)->car)
#define SCM_CDR(obj) (((ScmPair *)obj)->cdr)

#define SCM_TYPE_NULL      0
#define SCM_TYPE_INT       1
#define SCM_TYPE_SYMBOL    2
#define SCM_TYPE_STRING    3
#define SCM_TYPE_PAIR      4
#define SCM_TYPE_PORT      5
#define SCM_TYPE_PROCEDURE 6
#define SCM_TYPE_CLOSURE   7

struct ScmObject;
typedef struct ScmObject ScmObject;

typedef int scheme_int_t;
typedef unsigned char scheme_char_t;
typedef ScmObject *(*scheme_function_t)(ScmObject *);


typedef struct ScmObject {
    SCM_OBJECT_HEADER
} ScmObject;

typedef struct ScmInt {
    SCM_OBJECT_HEADER
    scheme_int_t val;
} ScmInt;

typedef struct ScmString {
    SCM_OBJECT_HEADER
    size_t len;
    const scheme_char_t *s;
} ScmString;

typedef struct ScmSymbol {
    SCM_OBJECT_HEADER
    ScmString *name;
} ScmSymbol;

typedef struct ScmPair {
    SCM_OBJECT_HEADER
    ScmObject *car;
    ScmObject *cdr;
} ScmPair;

typedef struct ScmProcedure {
    SCM_OBJECT_HEADER
    scheme_function_t fn;
} ScmProcedure;

typedef struct SCM {
    ScmObject *env;
    ScmObject *symbols;
} SCM;

SCM *scheme;

int scheme_init();
int scheme_eval(ScmObject *obj);
void scheme_register(ScmObject *symbol, ScmObject *obj);

// scheme object constructors
ScmObject * scheme_int(int i);
ScmObject * scheme_string(const char *s);
ScmObject * scheme_symbol(const char *name);
ScmObject * scheme_cons(ScmObject *car, ScmObject *cdr);
ScmObject * scheme_procedure(scheme_function_t fn);

// string functions
int scheme_strcmp(const ScmObject *obj, const char *s);

// scheme procedures
ScmObject * scheme_print(ScmObject *o);

#endif  /* _SCHEME_H_ */

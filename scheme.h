#ifndef _SCHEME_H_
#define _SCHEME_H_
#include <stdlib.h>

#define scheme_malloc malloc
#define scheme_free free
#define SCM_NULL NULL

#define SCM_OBJECT_HEADER scheme_tag_t tag;
#define SCM_TYPE(OBJ) ((OBJ != SCM_NULL) ? OBJ->tag : SCM_TYPE_NULL)

#define SCM_NEW_OBJECT(SCM_TYPE) \
    (SCM_TYPE *)scheme_malloc(sizeof(SCM_TYPE))

#define SCM_SET_TAG(OBJ, TAG) \
    do { \
        OBJ->tag = TAG; \
    } while(0)

#define SCM_INT(obj) (((ScmInt *)obj)->val)
#define SCM_SYMBOL_NAME(obj) (ScmObject *)(((ScmSymbol *)obj)->name)

#define SCM_CAR(obj) (((ScmPair *)obj)->car)
#define SCM_CDR(obj) (((ScmPair *)obj)->cdr)
#define SCM_CAAR(obj) SCM_CAR(SCM_CAR(obj))
#define SCM_CADR(obj) SCM_CAR(SCM_CDR(obj))
#define SCM_CDAR(obj) SCM_CDR(SCM_CAR(obj))
#define SCM_CDDR(obj) SCM_CDR(SCM_CDR(obj))
#define SCM_CADDR(obj) SCM_CAR(SCM_CDDR(obj))
#define SCM_CDDDR(obj) SCM_CDR(SCM_CDDR(obj))

#define SCM_TYPE_NULL      0
#define SCM_TYPE_INT       1
#define SCM_TYPE_SYMBOL    2
#define SCM_TYPE_STRING    3
#define SCM_TYPE_PAIR      4
#define SCM_TYPE_PORT      5
#define SCM_TYPE_PROCEDURE 6
#define SCM_TYPE_CLOSURE   7
#define SCM_TYPE_SYNTAX    8

struct ScmObject;
typedef struct ScmObject ScmObject;

typedef int scheme_int_t;
typedef unsigned long scheme_uint_t;
typedef unsigned long scheme_tag_t;
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

typedef struct ScmSyntax {
    SCM_OBJECT_HEADER
    scheme_function_t fn;
} ScmSyntax;

typedef struct SCM {
    ScmObject *env;
    ScmObject *symbols;
} SCM;

SCM *scheme;

int scheme_init();
ScmObject *scheme_eval(ScmObject *obj);
void scheme_register(const char *name, ScmObject *obj);
ScmObject *scheme_apply(ScmObject *proc, ScmObject *args);
int scheme_print_object(ScmObject *obj);
void scheme_error(const char *message);

// scheme object constructors
ScmObject *scheme_int(int i);
ScmObject *scheme_string(const char *s);
ScmObject *scheme_symbol(const char *name);
ScmObject *scheme_cons(ScmObject *car, ScmObject *cdr);
ScmObject *scheme_procedure(scheme_function_t fn);
ScmObject *scheme_syntax(scheme_function_t fn);

// string functions
int scheme_strcmp(const ScmObject *obj, const char *s);

// syntax functions
ScmObject *scheme_if(ScmObject *expr);

// scheme procedures
ScmObject *scheme_print(ScmObject *args);
ScmObject *scheme_plus(ScmObject *args);
ScmObject *scheme_minus(ScmObject *args);

#endif  /* _SCHEME_H_ */

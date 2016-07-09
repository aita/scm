#ifndef _SCHEME_H_
#define _SCHEME_H_
#include <stdlib.h>

#define scheme_malloc malloc
#define scheme_free free
#define SCM_NULL NULL

#define SCM_OBJECT_HEADER scheme_tag_t tag;
#define SCM_NEW(SCM_TYPE) \
    (SCM_TYPE *)scheme_malloc(sizeof(SCM_TYPE))

#define SCM_TAG(obj) (obj->tag)
#define SCM_SET_TAG(obj, tag) \
    do { \
        SCM_TAG(obj) = tag;  \
    } while(0)

#define SCM_TYPE(obj) ((obj != SCM_NULL) ? SCM_TAG(obj) : SCM_TYPE_NULL)

#define SCM_INT(obj) (((ScmInt *)obj)->val)
#define SCM_SYMBOL_NAME(obj) (ScmObject *)(((ScmSymbol *)obj)->name)
#define SCM_ERROR_MESSAGE(obj) (ScmObject *)(((ScmError *)obj)->message)

#define SCM_NULLP(obj) (obj == SCM_NULL)

#define SCM_TYPE_CHECK(type, obj) (SCM_TYPE(obj) == SCM_TYPE_##type)
#define SCM_INTP(obj) SCM_TYPE_CHECK(INT, obj)
#define SCM_SYMBOLP(obj) SCM_TYPE_CHECK(SYMBOL, obj)
#define SCM_STRINGP(obj) SCM_TYPE_CHECK(STRING, obj)
#define SCM_PAIRP(obj) SCM_TYPE_CHECK(PAIR, obj)
#define SCM_PORTP(obj) SCM_TYPE_CHECK(PORT, obj)
#define SCM_PROCEDUREP(obj) SCM_TYPE_CHECK(PROCEDURE, obj)
#define SCM_CLOSUREP(obj) SCM_TYPE_CHECK(CLOSURE, obj)
#define SCM_SYNTAXP(obj) SCM_TYPE_CHECK(SYNTAX, obj)
#define SCM_ERRORP(obj) SCM_TYPE_CHECK(ERROR, obj)

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
#define SCM_TYPE_ERROR     9

struct ScmObject;
typedef struct ScmObject ScmObject;

typedef int scheme_int_t;
typedef unsigned long scheme_uint_t;
typedef unsigned long scheme_tag_t;
typedef unsigned char scheme_char_t;
typedef ScmObject *(*scheme_function_t)(ScmObject *);
typedef ScmObject *(*scheme_syntax_t)(ScmObject *, ScmObject *);

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

typedef struct ScmSyntax {
    SCM_OBJECT_HEADER
    scheme_syntax_t fn;
} ScmSyntax;

typedef struct ScmProcedure {
    SCM_OBJECT_HEADER
    scheme_function_t fn;
} ScmProcedure;

typedef struct ScmClosure {
    SCM_OBJECT_HEADER
    ScmObject *params;
    ScmObject *body;
} ScmClosure;

typedef struct ScmError {
    SCM_OBJECT_HEADER
    ScmString *message;
} ScmError;

typedef struct SCHEME {
    ScmObject *env;
    ScmObject *symbols;
} SCHEME;

extern SCHEME *scheme;

int scheme_init();
void scheme_register(const char *name, ScmObject *obj);
void scheme_define(ScmObject *symbol, ScmObject *value, ScmObject *env);
ScmObject *scheme_eval(ScmObject *obj, ScmObject *env);
ScmObject *scheme_apply(ScmObject *proc, ScmObject *args, ScmObject *env);

// scheme object constructors
ScmObject *scheme_int(int i);
ScmObject *scheme_string(const char *s);
ScmObject *scheme_symbol(const char *name);
ScmObject *scheme_cons(ScmObject *car, ScmObject *cdr);
ScmObject *scheme_syntax(scheme_syntax_t fn);
ScmObject *scheme_procedure(scheme_function_t fn);
ScmObject *scheme_closure(ScmObject *params, ScmObject *body);

// utility functions
ScmObject *scheme_error(const char *message);
int scheme_print_object(ScmObject *obj);

// string functions
int scheme_strcmp(const ScmObject *obj, const char *s);

// scheme procedures
ScmObject *scheme_print(ScmObject *args);
ScmObject *scheme_plus(ScmObject *args);
ScmObject *scheme_minus(ScmObject *args);

#endif  /* _SCHEME_H_ */

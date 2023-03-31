#ifndef BORK_VALUE
#define BORK_VALUE

#include "common.h"

//A value has no notion of name, it is simply a _PRIMITIVE_ unit.

typedef enum{
    VAL_I32,
    VAL_U32,
    VAL_CHAR,
    VAL_OBJ_PTR,
} VALUE_TYPE;

typedef struct {
    VALUE_TYPE          type;
    union {
        uint32_t        U32;
        int32_t         I32;
        char            CHAR;
        void*           OBJ_PTR;
    } read_as;
} Value;

#endif
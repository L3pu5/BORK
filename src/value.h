#ifndef BORK_VALUE
#define BORK_VALUE

#include "common.h"

//A value has no notion of name, it is simply a _PRIMITIVE_ unit.

typedef enum{
    VAL_I32,
    VAL_U32,
    VAL_OBJ,
} VALUE_TYPE;

typedef enum {
    OBJ_STRING,
} OBJECT_TYPE;

typedef struct {
    OBJECT_TYPE         type;
} Object;

typedef struct {
    Object              object;
    int                 length;
    char*               string;
} Object_String;

typedef struct {
    VALUE_TYPE          type;
    union {
        uint32_t        U32;
        int32_t         I32;
        char            CHAR;
        void*           OBJ_PTR;
    } read_as;
} Value;

void value_to_string(Value value, char* buffer);
void free_value(Value value);

#endif
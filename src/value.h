#ifndef BORK_VALUE
#define BORK_VALUE

#include "common.h"
#include "./bark_objects/object.h"

//A value has no notion of name, it is simply a _PRIMITIVE_ unit.

typedef enum{
    VAL_I32,
    VAL_U32,
    VAL_OBJ,
    VAL_STR,
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
    char*               start;
} Object_String;

typedef struct {
    VALUE_TYPE          type;
    union {
        uint32_t        U32;
        int32_t         I32;
        char            CHAR;
        Object*         OBJ_PTR;
    } read_as;
} Value;


//Strings
void value_to_string(Value value, char* buffer);
Object* Object_create_string(char* source, int length);

void value_free(Value value);

#endif
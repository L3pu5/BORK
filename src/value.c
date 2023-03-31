#include <stdlib.h>
#include <stdio.h>

#include "value.h"

void value_to_string(Value value, char* buffer){
    switch(value.type){
        case VAL_I32:
            itoa(value.read_as.I32, buffer, 10);
        break;
        default:
            printf("Unimplemented to_string() method for type %i", value.type);
        break;
    }
}

void free_object(Object *object){
    switch(object->type){
        case OBJ_STRING:
            free( ((Object_String*) &object)->string);
            free(object);
        default:
            break;
    }
}

void free_value(Value value){
    switch(value.type){
        case VAL_OBJ:
            free_object(( (Object*) value.read_as.OBJ_PTR));
        default:
            break;
    }
}
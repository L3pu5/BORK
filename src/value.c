#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "value.h"

void value_to_string(Value value, char* buffer){
    switch(value.type){
        case VAL_I32:
            itoa(value.read_as.I32, buffer, 10);
        break;
        case VAL_STR:
            Object_String* stringObject = ((Object_String*) (value.read_as.OBJ_PTR));
            char* intermediateString =  stringObject->start;
            strncpy(buffer, intermediateString, stringObject->length);
        break;
        default:
            printf("Unimplemented to_string() method for type %i\n", value.type);
        break;
    }
}

//          STRING
//Instantiate String
Object* Object_create_string(char* source, int length){
    Object* rObject = calloc(1, sizeof(OBJ_STRING));
    rObject->type = OBJ_STRING;
    Object_String* rString = (Object_String*) rObject;
    rString->length = length;
    rString-> start = calloc(length, sizeof(char));
    strncpy(rString->start, source, rString->length);
    return rObject;
}

//FREE STRING
void free_object(Object *object){
    switch(object->type){
        case OBJ_STRING:
            free( ((Object_String*) &object)->start);
            free(object);
        default:
            break;
    }
}
//          /STRING


void value_free(Value value){
    switch(value.type){
        case VAL_OBJ:
            free_object(( (Object*) value.read_as.OBJ_PTR));
        default:
            break;
    }
}
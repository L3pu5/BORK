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
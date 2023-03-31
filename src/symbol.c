#include <stdlib.h>

#include "symbol.h"
#include "common.h"

void SymbolTable_init(SymbolTable* tbl){
    tbl->capacity   = DEFAULT_SYMBOLTABLE_CAPACITY;
    tbl->count      = 0;
    if(tbl->entries != NULL){
        free(tbl->entries);
    }
    tbl->entries    = malloc(DEFAULT_SYMBOLTABLE_CAPACITY * sizeof(Symbol));
    return;
}
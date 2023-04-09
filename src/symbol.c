#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symbol.h"
#include "common.h"
#include "value.h"

void SymbolTable_init(SymbolTable* tbl){
    tbl->capacity   = DEFAULT_SYMBOLTABLE_CAPACITY;
    tbl->count      = 0;
    tbl->entries    = calloc(DEFAULT_SYMBOLTABLE_CAPACITY, sizeof(Symbol));
    return;
}

// void Symbol_set_name(Symbol* symbol, char name[]){
//     symbol->name = name;
// }

void SymbolTable_dump(SymbolTable* tbl){
    printf("Dumping symbol table \n");
    for (int i = 0; i < tbl->capacity; i++)
    {
        printf("%04i - %s\n", i, tbl->entries[i].name);
    }
    printf("---------\n");
}

Symbol SymbolTable_get_by_index(SymbolTable* tbl, uint8_t index){
    return tbl->entries[index];
}

uint8_t SymbolTable_push(SymbolTable* tbl, Symbol s){
    if(tbl->count +1 >= tbl->capacity){
        tbl->entries = realloc(tbl->entries, tbl->capacity *2);
        tbl->capacity *= 2;
    }
    tbl->count++;
    //Find an empty spot.
    for (uint8_t i = 0; i < tbl->capacity; i++)
    {
       // printf("Attempting to index Symboltable at %d\n", i);
       // printf("Entries located at %p\n", tbl->entries);
       // printf("Ptr of i: -> %p\n", &tbl->entries[i]);
       // printf("TYPE: %d, comparing against %d", tbl->entries[i].type, SYMBOL_TOMBSTONE);
        //exit(69);
        if(tbl->entries[i].type == SYMBOL_TOMBSTONE){
           // printf("HERE!!!!");
            tbl->entries[i] = s;
            tbl->entries[i].value = s.value;
            //printf("Exiting push");
            return i;
        }

    }
    return -1;
}

Symbol* SymbolTable_get(SymbolTable* tbl, char* name){
    for (size_t i = 0; i < tbl->capacity; i++)
    {
        if(tbl->entries[i].type != SYMBOL_TOMBSTONE){
            if(strlen(tbl->entries[i].name) == strlen(name)){
                if(strcmp(tbl->entries[i].name, name) == 0){
                    return &tbl->entries[i];
                }
            }
        }
    }
    return NULL;
}

uint8_t SymbolTable_get_index_of(SymbolTable* tbl, char* name){
    for (size_t i = 0; i < tbl->capacity; i++)
    {
        if(tbl->entries[i].type != SYMBOL_TOMBSTONE){
            if(strlen(tbl->entries[i].name) == strlen(name)){
                if(strcmp(tbl->entries[i].name, name) == 0){
                    printf("INDEX_OF: %lli\n", i);
                    return i;
                }
            }
        }
    }
    return -1;
}

void SymbolTable_free(SymbolTable* tbl){
    printf("FREEING SYMBOL TABLE\n");
    for (size_t i = 0; i < tbl->capacity; i++)
    {
        if(tbl->entries[i].type != SYMBOL_TOMBSTONE){
            free_value(*tbl->entries[i].value);
        }
    }  
}
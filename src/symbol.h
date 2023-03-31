#ifndef BORK_SYMBOL
#define BORK_SYMBOL

#include "common.h"
#include "value.h"

typedef enum {
    SYMBOL_TOMBSTONE,
    SYMBOL_VALUE,
    
} SYMBOL_TYPE;

typedef struct {
    char            name[100];
    uint8_t         nameLength;
    SYMBOL_TYPE     type;
    Value*          value;
} Symbol;

typedef struct {
    int         capacity;
    int         count;
    Symbol*     entries;
} SymbolTable;

void SymbolTable_init   (SymbolTable* tbl);
uint8_t SymbolTable_push    (SymbolTable* tbl, Symbol symbol);
Symbol* SymbolTable_get    (SymbolTable* tbl, char* name);
void SymbolTable_free   (SymbolTable* tbl);
Symbol SymbolTable_get_by_index (SymbolTable* tbl, uint8_t index);
void SymbolTable_dump   (SymbolTable* tbl);

#endif 
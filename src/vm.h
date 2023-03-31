#ifndef BORK_VM
#define BORK_VM

#include "segment.h"
#include "common.h"
#include "value.h"
#include "symbol.h"

typedef struct {
    Byte*           ip;             // Instruction Pointer.
    Segment*        current;
    Value           stack[DEFAULT_VM_STACK_CAPACITY];
    Value*          stackPtr;
    SymbolTable*    globals;       
} VM;

void VM_init();
void VM_execute(Segment* seg);
void VM_walkStack();

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "math.h"
#include "vm.h"
#include "common.h"
//#include "debug.h"


VM vm;

void VM_init(){
    vm.ip = NULL;
    vm.stackPtr = vm.stack;
    // if(vm.globals != NULL){
    //     free(vm.globals);
    // }
    vm.globals = calloc(1, sizeof(SymbolTable));
    SymbolTable_init(vm.globals);
    //printf("Initialised VM\n");
}

// static void advance(){
//     vm.ip++;
// }

static Byte NEXT_BYTE(){
    Byte v = *vm.ip;
    vm.ip++;
    return v;
}

static void push(Value value){
    *vm.stackPtr = value;
    vm.stackPtr++;
}

static Value pop(){
    vm.stackPtr--;
    return *vm.stackPtr;
}

static void constant(){
    Value v = vm.current->constants[NEXT_BYTE()];
    push(v);
}

//Pop the last two values from the stack and add them pushing the result.
static void add(){
    Value v1  = pop();
    Value v2  = pop();
    switch(v1.type){
        case VAL_I32:
            int32_t v1_i32 = v1.read_as.I32;
            int32_t v2_i32 = v2.read_as.I32;
            Value output = {.type = VAL_I32, .read_as.I32 = (v1_i32 + v2_i32)};
            push(output);
            break;
        default:
            printf("Unimplemented operation.\n");
            return;
    }
}

static void bark(){
    char buffer[256];
    Value intermediateValue = pop();
    value_to_string(intermediateValue, buffer);
    if(intermediateValue.type == VAL_STR){
        printf("%.*s\n", ((Object_String*) intermediateValue.read_as.OBJ_PTR)->length, buffer);
    }else{
        printf("%s\n", buffer);
    }
}

static void sub(){
    Value v1  = pop();
    Value v2  = pop();
    switch(v1.type){
        case VAL_I32:
            int32_t v1_i32 = v1.read_as.I32;
            int32_t v2_i32 = v2.read_as.I32;
            Value output = {.type = VAL_I32, .read_as.I32 = (v1_i32 - v2_i32)};
            push(output);
            break;
        default:
            printf("Unimplemented operation.\n");
            return;
    }
}

static void mul(){
    Value v1  = pop();
    Value v2  = pop();
    switch(v1.type){
        case VAL_I32:
            int32_t v1_i32 = v1.read_as.I32;
            int32_t v2_i32 = v2.read_as.I32;
            Value output = {.type = VAL_I32, .read_as.I32 = (v1_i32 * v2_i32)};
            push(output);
            break;
        default:
            printf("Unimplemented operation.\n");
            return;
    }
}

//Renamed to avoid conflict with std::div
static void bork_div(){
    Value v1  = pop();
    Value v2  = pop();
    switch(v1.type){
        case VAL_I32:
            int32_t v1_i32 = v1.read_as.I32;
            int32_t v2_i32 = v2.read_as.I32;
            Value output = {.type = VAL_I32, .read_as.I32 = (v2_i32 / v1_i32)};
            push(output);
            break;
        default:
            printf("Unimplemented operation.\n");
            return;
    }
}

static void power(){
    Value v1  = pop();
    Value v2  = pop();
    switch(v1.type){
        case VAL_I32:
            int32_t v1_i32 = v1.read_as.I32;
            int32_t v2_i32 = v2.read_as.I32;
            Value output = {.type = VAL_I32, .read_as.I32 = pow(v2_i32, v1_i32)};
            push(output);
            break;
        default:
            printf("Unimplemented operation.\n");
            return;
    }
}

static void ret(){
    //printf("RETURN\n");
    return;
}

void VM_walkStack(){
    #ifdef BORK_VM_TRACE
    printf("Beginning stackwalk\n");
    printf("----------------\n");
    Value* walker = vm.stack;
    while(walker < vm.stackPtr){
        printf("Value: %i\n", (*walker).read_as.I32);
        walker++;
    }
    printf("----------------\n");
    #endif
}

static void push_var(Segment *seg){
    //printf("PUSHING VAR\n");
    uint8_t localIndex = NEXT_BYTE();
    //printf("%i\n", localIndex);
    Symbol s = SymbolTable_get_by_index(vm.globals, localIndex);
    push(*(s.value));
    //printf("Pushed\n");
    //printf("NAME OF SYMBOL S: %s, Value %p\n", s.name, s.value->read_as.OBJ_PTR);
}

//Assignment
static void global_SVAR(Segment* seg){
    uint8_t localIndex = NEXT_BYTE();
    //printf("INDEX %i - %s\n", localIndex, SymbolTable_get_by_index(seg->symbols, 0).name);
    Value v = pop();
    Value* v_value = calloc(1, sizeof(Value));
    memcpy(v_value, &v, sizeof(Value));
    //printf("Memcpy in place");
    Symbol s = SymbolTable_get_by_index(seg->symbols, localIndex);
    s.value = v_value;
    //printf("VALUE POINTER ON ASSIGN: %p\n", s.value->read_as.OBJ_PTR);
    //printf("GOT %s, %i TYPE\n", s.name, s.type);
    SymbolTable_push(vm.globals, s);
    //SymbolTable_dump(vm.globals);
    //printf("PUshed onto SymbolTable");

//    printf("ALLOCATED GLOBAL %.*s - Value %i", SymbolTable_get(vm.globals, "x").nameLength, SymbolTable_get(vm.globals, "x")->.name, SymbolTable_get(vm.globals, "x")->value->read_as.I32);
}


static void global_I32(Segment* seg){
    //Expect OP_DEF_I32 then the idnex inside the segment's ymbol table
    //SymbolTable_dump(seg->symbols);
    //VM_walkStack();
    uint8_t localIndex = NEXT_BYTE();
    //printf("INDEX %i - %s\n", localIndex, SymbolTable_get_by_index(seg->symbols, 0).name);
    Value v = pop();
    Value* v_value = calloc(1, sizeof(Value));
    memcpy(v_value, &v, sizeof(Value));
    //printf("Memcpy in place");
    //printf("READING %i\n", v_value->read_as.I32);
    Symbol s = SymbolTable_get_by_index(seg->symbols, localIndex);
    s.value = v_value;
    //printf("VALUE POINTER ON ASSIGN: %i\n", s.value->read_as.I32);
    //printf("GOT %s, %i TYPE\n", s.name, s.type);
    SymbolTable_push(vm.globals, s);

//    printf("ALLOCATED GLOBAL %.*s - Value %i", SymbolTable_get(vm.globals, "x").nameLength, SymbolTable_get(vm.globals, "x")->.name, SymbolTable_get(vm.globals, "x")->value->read_as.I32);
}

void VM_execute(Segment* seg){
    vm.ip = seg->code;
    vm.current = seg;

    //OpCode code = *vm.ip;
    for (;;){
        OpCode code = NEXT_BYTE();
        #ifdef BORK_VM_TRACE
        printf("VM -> %i\n", code);
        printf("VM : Instruction %04i\n", code);
        printf("   | Instruction %s\n", OpCode_Disassemble(code));
        #endif
        //Execute the instruction of the OpCode
        switch(code){
            case OP_CONSTANT:
                constant();
                break;
            case OP_ADD:
                add();
                break;
            case OP_SUB:
                sub();
                break;
            case OP_MUL:
                mul();
                break;
            case OP_DIV:
                bork_div();
                break;
            case OP_POW:
                power();
                break;
            case OP_DEF_I32:
                global_I32(seg);
                break;
            case OP_DEF_SVAR:
                global_SVAR(seg);
                break;
            case OP_ID:
                push_var(seg);
                break;
            case OP_POP:
                pop();
                break;
            case OP_BARK:
                bark();
                break;
            case OP_RET:
                //For now we will just print the stack
                ret();
                return;
            default:
                printf("Unexpected instruction %i\n", code);
                printf("VM : Instruction %04i\n", code);
                printf("   | Instruction %s\n", OpCode_Disassemble(code));
                return;
        }

        #ifdef BORK_VM_TRACE
            printf("WALKING STACK AFTER OPERATIONS\n");
            VM_walkStack();
        #endif

        //advance();
    }
}
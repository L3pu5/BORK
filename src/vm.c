#include <stdio.h>

#include "math.h"
#include "vm.h"
#include "common.h"

VM vm;

void VM_init(){
    vm.ip = NULL;
    vm.stackPtr = vm.stack;
    printf("Initialised VM\n");
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
            printf("Unimplemented operation.");
            return;
    }
}

static void sub(){
    Value v1  = pop();
    Value v2  = pop();
    switch(v1.type){
        case VAL_I32:
            int32_t v1_i32 = v1.read_as.I32;
            int32_t v2_i32 = v2.read_as.I32;
            Value output = {.type = VAL_I32, .read_as.I32 = (v2_i32 - v1_i32)};
            push(output);
            break;
        default:
            printf("Unimplemented operation.");
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
            printf("Unimplemented operation.");
            return;
    }
}

static void div(){
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
            printf("Unimplemented operation.");
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
            printf("Unimplemented operation.");
            return;
    }
}

static void ret(){
    VM_walkStack();
    return;
}

void VM_walkStack(){
    printf("Beginning stackwalk\n");
    printf("----------------\n");
    Value* walker = vm.stack;
    while(walker < vm.stackPtr){
        printf("Value: %i\n", (*walker).read_as.I32);
        walker++;
    }
    printf("----------------\n");
}

void VM_execute(Segment* seg){
    vm.ip = seg->code;
    vm.current = seg;

    //OpCode code = *vm.ip;
    for (;;){
        OpCode code = NEXT_BYTE();
        printf("VM -> %i\n", code);
        
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
                div();
                break;
            case OP_POW:
                power();
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

        printf("VM : Instruction %04i\n", code);
        printf("   | Instruction %s\n", OpCode_Disassemble(code));
        //advance();
    }
}
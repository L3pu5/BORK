#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "segment.h"
#include "debug.h"
#include "value.h"


ParseRule Parse_Rules[];
static void parse(Segment* seg, Order order);
static ParseRule getRule(TOKEN_TYPE type);

const char* OpCode_Disassemble(OpCode op){
    switch(op){
        case OP_ADD:
            return "ADD";
        case OP_SUB:
            return "SUB";
        case OP_MUL:
            return "MUL";
        case OP_DIV:
            return "DIV";
        case OP_CONSTANT:
            return "CNST";
        case OP_BARK:
            return "BARK";
        case OP_RET:
            return "RET";
        case OP_DEF_I32:
            return "DEF_I32";
        default:
            printf("Unimplemented OpCode Disassemble\n");
            //exit(12);
            return "NOT IMPLEMENTED";
    }
}

void Segment_init(Segment* seg){
    seg->code           = malloc(DEFAULT_OPCODE_CAPACITY * sizeof(Byte));
    seg->constants      = malloc(DEAFULT_CONSTANT_CAPACITY * sizeof(Value));
    seg->codeCapacity           = DEFAULT_OPCODE_CAPACITY;
    seg->constantCapacity       = DEAFULT_CONSTANT_CAPACITY;
    seg->codeCount              = 0;
    seg->constantCount          = 0;
    seg->ip                     = NULL;
    seg->ipIndex                = 0;
    seg->symbols                = calloc(1, sizeof(SymbolTable));
    SymbolTable_init(seg->symbols);
    return;
}

void Segment_free(Segment* seg){
    free(seg->code);
    free(seg->constants);
    SymbolTable_free(seg->symbols);
    free(seg->symbols);
    return;
}

void Segment_writeByte(Segment* seg, Byte byte){
    if(seg->codeCapacity <= (seg->codeCount+1)) {
        seg->code = realloc(seg->code, seg->codeCapacity *2);
        seg->codeCapacity *= 2;
    }
    seg->code[seg->codeCount] = byte;
    seg->codeCount++;
    return;
}

void Segment_writeBytes(Segment* seg, Byte b1, Byte b2){
    Segment_writeByte(seg, b1);
    Segment_writeByte(seg, b2);
    return;
}

int Segment_pushConstant(Segment* seg, Value constant){
    if(seg->constantCapacity <= (seg->constantCount+1)) {
        seg->constants = realloc(seg->constants, seg->constantCapacity *2);
        seg->constantCapacity *= 2;
    }
    //printf("Dumping a constant %i in %i\n", constant, seg->constantCount);
    seg->constants[seg->constantCount] = constant;
    seg->constantCount++;
    return seg->constantCount -1;
}

static void advance(Segment* seg){
    seg->ip++;
    seg->ipIndex++;
    //printf("Advanced pointer to index %i\nWe are looking at %i\n '%.*s'", seg->ipIndex, seg->ip->type, seg->ip->length, seg->ip->start);
    return;
}

static void consume(Segment* seg, TOKEN_TYPE type, const char* message){
    //printf("Looking at '%.*s' against %i\n", seg->ip->length, seg->ip->start, type);
    if(seg->ip->type == type)
    {
    //    printf("matched %i with %i\n", seg->ip->type, type);
        advance(seg);
        return;
    }
    printf("%s\n", message);
    return;
}

static bool match(Segment* seg, TOKEN_TYPE type){
    if(seg->ip[-1].type == type){
        advance(seg);
        return true;
    }
    return false;
}

static void expression(Segment* seg){
    //Go at the lowest level, we consume everything on the other side
    //of an :='ss
    printf("Current Token: %.*s\n", seg->ip->length, seg->ip->start);
    printf("Current Token: %i\n",  seg->ip->type);
    parse(seg, ORDER_ASSIGNMENT);
    consume(seg, TOKEN_SEMI, "Expected ';' after an expression.");
    return;
}


static void grouping(Segment* seg){
    expression(seg);
    consume(seg, TOKEN_PAREN_LEFT, "Expected ')' after expression.;");
    return;
}

static void expressionStatement(Segment* seg){
    expression(seg);
    Segment_writeByte(seg, OP_POP);
}

static void statement(Segment* seg){
    if( match(seg, TOKEN_BARK)){
            expression(seg);
            Segment_writeByte(seg, OP_BARK);
    }
    else{
        expressionStatement(seg);
    }
}

static void i32_declaration(Segment* seg){
    if(seg->ip[-1].type == TOKEN_ID){
        Token label = seg->ip[-1];
        //advance(seg);
        consume(seg, TOKEN_COLON, "Expected ':' after identifier.");
        advance(seg);
        printf("%.*s", seg->ip->length, seg->ip->start);
        expression(seg);

        //Write it to the symbol table
        //printf("LENGTH OF LABEL: %i\n", label.length);
        Symbol identifier = { .type = SYMBOL_VALUE};
        memcpy(identifier.name, label.start, label.length);
        identifier.name[label.length] = '\0';
        //printf("IDENTIFIER: %s, %i\n", identifier.name, identifier.type);
        uint8_t symbolTableIndex = SymbolTable_push(seg->symbols, identifier);
        //seg->symbols->entries[0].name = "TEST";

        //printf("TESTING SYMBOL %s = %s\n", identifier.name, seg->symbols->entries[symbolTableIndex].name);
        //printf("INDEX IS AT %i", symbolTableIndex);
        Segment_writeBytes(seg, OP_DEF_I32, symbolTableIndex);
        //SymbolTable_dump(seg->symbols);
        return;
    }else{
        printf("Expected '<id>' after i32");
    }
}

static void declaration(Segment* seg){
    if(match(seg, TOKEN_I32)){
        i32_declaration(seg);
    }
    else{
        statement(seg);
    }
}



static void binary(Segment* seg){
    TOKEN_TYPE operator = seg->ip[-1].type;
    //printf("Binary");
    
    ParseRule rule = getRule(operator);
    advance(seg);
    //printf("ORDER IN BINARY: %i\n", rule.order);
    //Parse everything of higher importance ie, following grouping on the right of the operator.
    parse(seg, rule.order);

    switch(operator){
        case TOKEN_PLUS:
            Segment_writeByte(seg, OP_ADD); break;
        case TOKEN_MINUS:
            Segment_writeByte(seg, OP_SUB); break;
        case TOKEN_SLASH:
            Segment_writeByte(seg, OP_DIV); break;
        case TOKEN_STAR:
            Segment_writeByte(seg, OP_MUL); break;
        case TOKEN_CARROT:
            Segment_writeByte(seg, OP_POW); break;
        default: break;
    }
    return;
}

static void unary(Segment* seg){
    //Grab the type of unary operator
    TOKEN_TYPE operator = seg->ip[-1].type;
    //Compile everything AFTER the operator
    expression(seg); //we can have -(ax + b)

    //Which type of operator are we?
    switch(operator){
        case TOKEN_MINUS:
            Segment_writeByte(seg, OP_NEG);
            break;
        default:
            break;
    }
    return;
}

static void number(Segment* seg){
    printf("NUMBER");
    char buffer[seg->ip[-1].length + 1];
    memcpy(buffer, seg->ip[-1].start, seg->ip[-1].length); 
    buffer[seg->ip[-1].length] = '\0';
    //By default, constants are I32;
    //printf("Considering the constant number '%s'", buffer);
    uint32_t v = atoi(buffer);
    Value constantValue = {.type = VAL_I32, .read_as.I32 = v};
    int index = Segment_pushConstant(seg, constantValue);
    Segment_writeBytes(seg, OP_CONSTANT, index);
    return;
}

// static void string(Segment* seg){
//     //Create the string
//     char* buffer = malloc(seg->ip[-1].length + 1* sizeof(char));
//     memcpy(buffer, seg->ip[-1].start, seg->ip[-1].length); 
//     buffer[seg->ip[-1].length] = '\0';
//     //Dump the string pointer into a new value
//     Object_String* string = malloc(sizeof(Object_String));
//     string->string = buffer;

//     Value constantValue = {.type = VAL_OBJ, .read_as.OBJ_PTR = string};
//     int index = Segment_pushConstant(seg, constantValue);
//     Segment_writeBytes(seg, OP_CONSTANT, index);
//     return;    
// }

// Parsing logic definition
// {PREFIX, INDFIX, ORDER}
// An expression MUST ALWAYS start with a prefix operator.
//Based on our prefix operator, we then move on.
ParseRule Parse_Rules[] = {
    [TOKEN_PLUS]        = {NULL,        binary,        ORDER_TERM},
    [TOKEN_MINUS]       = {unary,       binary,        ORDER_TERM},
    [TOKEN_SLASH]       = {NULL,        binary,        ORDER_FACTOR},
    [TOKEN_STAR]        = {NULL,        binary,        ORDER_FACTOR},
    [TOKEN_NUMBER]      = {number,        NULL,          ORDER_NONE},
    [TOKEN_PAREN_LEFT]  = {grouping,      NULL,          ORDER_NONE},
    [TOKEN_EOF]         = {NULL,          NULL,          ORDER_NONE},
    [TOKEN_SEMI]        = {NULL,          NULL,          ORDER_NONE},
    [TOKEN_CARROT]      = {NULL,        binary,         ORDER_POWER},
};

static ParseRule getRule(TOKEN_TYPE type){
    return Parse_Rules[type];
}

//We parse continuing from our current index
//ONLY accepting tokens of higher order.
//Order here is the order from the callee
static void parse(Segment* seg, Order order){
    //Get our rules.
    ParseRule currentRule = getRule(seg->ip[-1].type);
    //Are we a prefix operator?
    void *prefixOperation = currentRule.prefixOperation;

    if(prefixOperation == NULL){
        // We are not a prefix operator.
        //We should always start on a prefix operator
        //Whether that be a number, a variable, or a declaration
        #ifdef BORK_COMPILE_TRACE
        //printf("Expected prefix token, found %i", seg->ip[-1].type);
        #endif
        return;
    }
    //Perform a prefix operation.
    ((void (*)(Segment*)) prefixOperation)(seg);

    //Otherwise, we perform an INFIX operation IF and ONLY IF
    //The order/precedence of of our operation is HIGHER UP
   
    
    while( order <= getRule(seg->ip->type).order){
        //printf("Considering infix operation on token '%.*s'", seg->ip->length, seg->ip->start);
        //Text the next rule;
        advance(seg);
        void* infixOperation = getRule(seg->ip[-1].type).infixOperation;
        ((void (*)(Segment*)) infixOperation)(seg);
        //IGNORE
    }
}


void Segment_compile(Segment* seg, TokenStack* tStack){
    seg->ip         = tStack->tokens;
    seg->ipIndex    = 0;
    advance(seg);
    declaration(seg);
    while(seg->ip->type != TOKEN_EOF){
         advance(seg);
         declaration(seg);
    }
    consume(seg, TOKEN_EOF, "Expected the end of the file.");
    //Push a return
    // Test with an unexpected type.
    // Segment_writeByte(seg, OP_CONSTANT);
    // Value tempValue = {.type = VAL_U32, .read_as.U32 = 12};
    // int index_of_temp_value = Segment_pushConstant(seg, tempValue);
    // Segment_writeByte(seg, index_of_temp_value);
    // Segment_writeByte(seg, OP_SUB);
    Segment_writeByte(seg, OP_RET);

    #ifdef BORK_COMPILE_TRACE

    printf("Dumping opcode stack. %i codes\n", seg->codeCount);

    for(int i = 0; i < seg->codeCount; i++){
        printf("%04i %04i\n", i, seg->code[i]);
        // if(seg->code[i] == OP_CONSTANT){
        //     printf("->%i at constants[%i]\n", seg->constants[seg->code[i+1]].read_as.I32, seg->code[i+1]);
        //     i++;
        // }
    }
    if(seg->ip->type == TOKEN_EOF){
        printf("Finished reading the stack\n");
    }
    #endif
    //printf("DUMPING SYMBOLTABLE AT END OF COMPILE\n");
    //SymbolTable_dump(seg->symbols);
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "segment.h"
#include "debug.h"


ParseRule Parse_Rules[];
static void parse(Segment* seg, Order order);
static ParseRule getRule(TOKEN_TYPE type);

void Segment_init(Segment* seg){
    seg->code           = malloc(DEFAULT_OPCODE_CAPACITY * sizeof(Byte));
    seg->constants      = malloc(DEAFULT_CONSTANT_CAPACITY * sizeof(Value));
    seg->codeCapacity           = DEFAULT_OPCODE_CAPACITY;
    seg->constantCapacity       = DEAFULT_CONSTANT_CAPACITY;
    seg->codeCount              = 0;
    seg->constantCount          = 0;
    seg->ip                     = NULL;
    seg->ipIndex                = 0;
    return;
}

void Segment_free(Segment* seg){
    free(seg->code);
    free(seg->constants);
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
    if(seg->ip->type == type)
    {
        advance(seg);
        return;
    }
    printf("%s", message);
    return;
}

static void expression(Segment* seg){
    //Go at the lowest level, we consume everything on the other side
    //of an :='ss
    //printf("Current rule type %i\n", seg->ip->type);
    parse(seg, ORDER_ASSIGNMENT);
    consume(seg, TOKEN_SEMI, "Expected ';' after an expression.");
    return;
}


static void grouping(Segment* seg){
    expression(seg);
    consume(seg, TOKEN_PAREN_LEFT, "Expected ')' after expression.");
    return;
}


// static void declaration(Segment* seg){
//     return;
// }


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
    char buffer[seg->ip[-1].length];
    memcpy(buffer, seg->ip[-1].start, seg->ip[-1].length); 
    buffer[seg->ip[-1].length] = '\0';
    //printf("Considering the constant number '%s'", buffer);
    Value v = atoi(buffer);
    int index = Segment_pushConstant(seg, v);
    Segment_writeBytes(seg, OP_CONSTANT, index);
    return;
}

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
    expression(seg);
    consume(seg, TOKEN_EOF, "Expected the end of the file.\n");
    printf("Dumping opcode stack. %i codes\n", seg->codeCount);

    for(int i = 0; i < seg->codeCount; i++){
        printf("%04i %04i\n", i, seg->code[i]);
        if(seg->code[i] == OP_CONSTANT){
            printf("->%i at constants[%i]\n", seg->constants[seg->code[i+1]], seg->code[i+1]);
            i++;
        }
    }
    if(seg->ip->type == TOKEN_EOF){
        printf("Finished reading the stack\n");
    }
}
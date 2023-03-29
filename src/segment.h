#ifndef BORK_SEGMENT
#define BORK_SEGMENT

#include "common.h"
#include "symbol.h"
#include "lexer.h"

//One byte, up to 0xFF
typedef enum {
    OP_CONSTANT,
    OP_NEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_BARK,
    OP_RET,
} OpCode;

//A segment of compiled code
typedef struct {
    Byte*       code;
    int         codeCapacity;
    int         codeCount;
    
    Value*      constants;
    int         constantCapacity;
    int         constantCount;

    int         ipIndex;
    Token*      ip;
    int         maxIndex;
    //SymbolTable localConstants;
} Segment;

typedef enum {
    ORDER_NONE,         
    ORDER_ASSIGNMENT,   // :=
    ORDER_OR,           //  |
    ORDER_AND,          //  &
    ORDER_EQUALITY,     // =
    ORDER_COMPARISON,   // > < 
    ORDER_TERM,         // + -
    ORDER_FACTOR,       // * /
    ORDER_UNARY,        // - ~
    ORDER_CALL,         // 
    ORDER_PRIMARY       // Identifer/const
} Order;

//Parsing rules
typedef struct {
    void* prefixOperation;
    void* infixOperation;
    Order order;
} ParseRule;

void Segment_init(Segment* seg);
void Segment_free(Segment* seg);
void Segment_compile(Segment* seg, TokenStack* tStack);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "lexer.h"
#include "common.h"
#include "debug.h"

typedef struct {
    char*           current;
    char*           previous;
    const char*     input;
    TokenStack*     currentStack;
    uint16_t        line;
    uint16_t        char_offset;
} Lexer;

Lexer lex;

void TokenStack_init(TokenStack* tStack){
    tStack->count       = 0;
    tStack->capacity    = DEFAULT_TOKENSTACK_CAPACITY;
    tStack->tokens      = malloc(DEFAULT_TOKENSTACK_CAPACITY * sizeof(Token));
    return;
}

void Lexer_init(char* input){
    lex.input           = input;
    lex.previous        = input;
    lex.current         = input;
    lex.char_offset     = 0;
    lex.line            = 0;
    if(lex.currentStack != NULL){
        free(lex.currentStack);
    }
    lex.currentStack    = malloc(sizeof(TokenStack)); 
    TokenStack_init(lex.currentStack);
    return;
}

void TokenStack_free(TokenStack* tStack){
    free(tStack->tokens);
    return;
}

void TokenStack_push(TokenStack* tStack, Token token){
    //Do we have space on the stack?
    if(tStack->count + 1 >= tStack->capacity){
        // We need to create more room;
        tStack->tokens = realloc(tStack->tokens, tStack->capacity * 2);
        tStack->capacity *= 2;
        #ifdef BORK_PARSER_TRACE
        printf("We are expending the capcity of the TokenStack %i -> %i\n", tStack->capacity/2, tStack->capacity);
        #endif
    }
    //We push the new token;
    tStack->tokens[tStack->count].length =  token.length;
    tStack->tokens[tStack->count].type =    token.type;
    tStack->tokens[tStack->count].start =   token.start;
    tStack->count++;
    return;
}

void Token_print(Token* token){
    printf("%04i '%.*s'\n", token->type, token->length, token->start);
    return;
}

static char cursor() {
    return lex.current[0];
}

static char peek() {
    return lex.current[1];
}

static bool EoF(){
    return( peek() == '\0');
}

static bool isNumber(char c){
    return ( c >= '0' && c <= '9');
}

static void advance(){
    lex.current++;
    lex.char_offset++;
    return;
}

static Token makeToken(char* start, int length, TOKEN_TYPE type){
    Token t = {.start = start, .length = length, .type = type};
    return t;
}

static void number(){
    //Read ahead until the next character is NOT a number
    while(isNumber(peek())){
        advance();
    }
    //Create a new number token
    TokenStack_push(lex.currentStack, makeToken(lex.previous, lex.current - lex.previous + 1, TOKEN_NUMBER));
    return;
}



TokenStack* Lexer_parse() {
    //Read all the chracters
    for (;;) {
        //is this the end of the file?
        if( EoF()){
            TokenStack_push(lex.currentStack, makeToken(NULL, 0, TOKEN_EOF));
            return lex.currentStack;
        }
        // We set the previous character to the current one
        lex.previous = lex.current;
        //Is this a number?
        if( isNumber(cursor())){
            number();
            advance();
            continue;        
        }

        switch(cursor()){
            case '(':
                 TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_PAREN_LEFT));
                 break;
            case ')':
                 TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_PAREN_RIGHT));
                 break;
            case '+': 
                TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_PLUS));
                break;
            case '-':
                TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_MINUS));
                break;
            case '*':
                TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_STAR));
                break;
            case '/':
                TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_SLASH));
                break;
            case '\n':
                lex.line++;
                lex.char_offset = 0;
                break;
            case ';':
                TokenStack_push(lex.currentStack, makeToken(lex.current, 1, TOKEN_SEMI));
                break;
            default:
                printf("Unexpected character at <%i:%i> '%c'\n", lex.line, lex.char_offset, *lex.current);
                exit(42069420);
                break;
        }
        //Advance
        advance();
    }


    return lex.currentStack;
}


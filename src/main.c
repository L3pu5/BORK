#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "debug.h"
#include "segment.h"

int main(){
    printf("Hello world!\n");
    for (;;){
        //Get input, max 1024 char
        char inputBuffer[1024];
        fgets(inputBuffer, 1024, stdin);
        //Bail if exit
        if(memcmp(inputBuffer, "exit", 4) == 0){
            printf("Goodbye!");
            return 420;
        }
        //Initialise lexer based on input;
        Lexer_init(inputBuffer);
        TokenStack* tokens = Lexer_parse();
        #ifdef BORK_PARSER_TRACE
        printf("-------------------\n");
        printf("Count of tokens in tokenStack: %i\n", tokens->count);
        for (size_t i = 0; i < tokens->count; i++)
        {
            Token_print(&tokens->tokens[i]);
        }
        printf("-------------------\n");
        #endif
        Segment s;
        Segment_init(&s);
        Segment_compile(&s, tokens);
        //Free the TokenStack
        TokenStack_free(tokens);
    }
    return 69;
}
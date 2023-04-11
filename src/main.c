#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
//#include "debug.h"
#include "segment.h"
#include "vm.h"

int repl();
static void doFile(const char* path);

int main(int argc, char* argv[]){
    if(argc == 1){
        repl();
    }
    if(argc == 2){
        doFile(argv[1]);
    }
    printf("%i\n", argc);
    return 69;
}

static TokenStack* ParseCode(char* code){
    //Initialise lexer based on input;
    Lexer_init(code);
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
    return tokens;
}

static void Compile_and_execute_Segment(TokenStack* tokens){
    Segment s;
    Segment_init(&s);
    Segment_compile(&s, tokens);
    VM_init();
    VM_execute(&s);
    //printf("Execute complete.");
}

static char* readCode(const char* path){
    FILE* srcfile = fopen(path, "r");
    fseek(srcfile, 0, SEEK_END);
    int length = ftell(srcfile);
    fseek(srcfile, 0, SEEK_SET);
    char*soureCode = malloc(sizeof(char) * (length + 1)); 
    fread(soureCode, sizeof(char), length, srcfile);
    soureCode[length] = '\0';
    fclose(srcfile);
    return soureCode;
}

static void doFile(const char* path){
    char* code = readCode(path);
    //printf(code);
    TokenStack* tokens = ParseCode(code);
    Compile_and_execute_Segment(tokens);
    TokenStack_free(tokens);
    //free(code);
    return;
}

int repl(){
    for (;;){
        //Get input, max 1024 char
        char inputBuffer[1024];
        fgets(inputBuffer, 1024, stdin);
        //Bail if exit
        if(memcmp(inputBuffer, "exit", 4) == 0){
            printf("Goodbye!");
            return 420;
        }
        TokenStack* tokens = ParseCode(inputBuffer);
        Compile_and_execute_Segment(tokens);
        TokenStack_free(tokens);
        
    }
    return 69;
}
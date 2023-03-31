#ifndef bork_lexer
#define bork_lexer

typedef enum {
    //GROUPING
    TOKEN_PAREN_LEFT,
    TOKEN_PAREN_RIGHT,
    //Primitives
    TOKEN_NUMBER,
    TOKEN_I32,
    //ARITHMETIC
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_CARROT,
    //VARIABLES
    TOKEN_VAR,
    //Core
    TOKEN_COLON,
    TOKEN_SEMI,
    TOKEN_BARK,
    TOKEN_EOF,
} TOKEN_TYPE;

typedef struct {
    char*       start;
    int         length;
    TOKEN_TYPE   type;
} Token;

typedef struct {
    int         capacity;
    int         count;
    Token*      tokens;
} TokenStack;

void Lexer_init(char* input);
TokenStack* Lexer_parse();
void TokenStack_free(TokenStack* tStack);
void Token_print(Token* token);

#endif
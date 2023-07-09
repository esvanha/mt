#ifndef LEXER_H
#define LEXER_H

enum TokenType {
    IDENTIFIER, // e.g. 'let'
    L_PAREN, // '('
    R_PAREN, // ')'
    INTEGER_LITERAL, // e.g. '24'
    FLOAT_LITERAL, // e.g. '2.24'
    ATOM, // e.g. ':abc'
    QUOTE, // "'"
    END_OF_INPUT
};

typedef struct {
    enum TokenType token_type;
    union TokenValue {
        int int_value;
        char* str_value;
        float float_value;
    } value;
} Token;

typedef struct TokenListNode {
    struct TokenListNode* next_node;
    Token token;
} TokenListNode;

typedef struct {
    const char* input;
    int line_count;
    int column_count;
} Lexer;

void token_print(const Token* token);
void token_free(Token* token);
Lexer lexer_new(const char* input);
Token lexer_next_token(Lexer* lexer);

#endif

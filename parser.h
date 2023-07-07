#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "lexer.h"

typedef struct {
    Token* previous_token;
    Token* current_token;
    Token* next_token;
    Lexer* lexer;
} Parser;

Parser parser_new(Lexer* lexer);
Expression* parser_parse_expression(Parser* parser);
Expression* parser_parse_program(Parser* parser);
void parser_free(Parser* parser);

#endif

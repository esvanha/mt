#include "parser.h"
#include "util.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

Parser
parser_new(Lexer* lexer)
{
    return (Parser){
        .previous_token = NULL,
        .current_token = NULL,
        .next_token = NULL,
        .lexer = lexer
    };
}

static Token*
parser_next_token(Parser* parser)
{
    if (parser->previous_token != NULL)
    {
        token_free(parser->previous_token);
        SAFE_FREE(parser->previous_token);
    }

    if (parser->current_token != NULL)
    {
        parser->previous_token = parser->current_token;
    }

    if (parser->next_token != NULL)
    {
        parser->current_token = parser->next_token;
        parser->next_token = NULL;
    }
    else
    {
        parser->current_token = malloc(sizeof(Token));
        if (parser->current_token == NULL)
        {
            // TODO: error returnen
            assert(false);
        }

        *(parser->current_token) = lexer_next_token(parser->lexer);
    }

   return parser->current_token;
}

static void
parser_rewind_token(Parser* parser)
{
    if (parser->next_token != NULL)
    {
        // TODO: better error handling/message
        fprintf(stderr, "Can't rewind more than one token.");
        exit(EXIT_FAILURE);
    }

    parser->next_token = parser->current_token;
    parser->current_token = parser->previous_token;
    parser->previous_token = NULL;
}

static bool
parser_accept(Parser* parser, enum TokenType token_type)
{
    Token* token = parser_next_token(parser);

    parser_rewind_token(parser);
    
    return token->token_type == token_type;
}

static Token*
parser_expect(Parser* parser, enum TokenType token_type)
{
    if (!parser_accept(parser, token_type))
    {
        // TODO: better error handling, better error message
        fprintf(stderr, "Expected different token type");
        exit(EXIT_FAILURE);
    }

    return parser_next_token(parser);
}

static Expression*
parser_parse_identifier(Parser *parser)
{
    Token* token = parser_expect(parser, IDENTIFIER);

    return expression_new(
        IDENTIFIER_EXPR, 
        (union ExpressionValue){ .str_value = dupstr(token->value.str_value) }
    );
}

static Expression*
parser_parse_list(Parser *parser)
{
    parser_expect(parser, L_PAREN);

    ExpressionListBuilder list_builder = expression_list_builder_new();

    // TODO: enum values powers of 2 maken zodat je END_OF_INPUT | R_PAREN kunt doen?
    while (
        !parser_accept(parser, END_OF_INPUT)
        && !parser_accept(parser, R_PAREN)
    ) {
        expression_list_builder_add(
            &list_builder,
            parser_parse_expression(parser)
        );
    }

    parser_expect(parser, R_PAREN);

    return expression_list_builder_build_expr(&list_builder);
}

static Expression*
parser_parse_float(Parser* parser)
{
    Token* token = parser_expect(parser, FLOAT_LITERAL);

    return expression_new(
        FLOAT_EXPR, 
        (union ExpressionValue){ .float_value = token->value.float_value }
    );
}

static Expression*
parser_parse_atom(Parser* parser)
{
    Token* token = parser_expect(parser, ATOM);

    return expression_new(
        ATOM_EXPR, 
        (union ExpressionValue){ .str_value = dupstr(token->value.str_value) }
    );
}

static Expression*
parser_parse_quoted_expression(Parser* parser)
{
    parser_expect(parser, QUOTE);

    return expression_new(
        QUOTED_EXPR,
        (union ExpressionValue){
            .quoted_expression = parser_parse_expression(parser)
        }
    );
}

static Expression*
parser_parse_integer(Parser* parser)
{
    Token* token = parser_expect(parser, INTEGER_LITERAL);

    return expression_new(
        INTEGER_EXPR, 
        (union ExpressionValue){ .int_value = token->value.int_value }
    );
}

Expression*
parser_parse_expression(Parser* parser)
{
//    Token* token;

    if (parser_accept(parser, IDENTIFIER))
    {
        return parser_parse_identifier(parser);
    }
    else if (parser_accept(parser, L_PAREN))
    {
        return parser_parse_list(parser);
    }
    else if (parser_accept(parser, INTEGER_LITERAL))
    {
        return parser_parse_integer(parser);
    }
    else if (parser_accept(parser, FLOAT_LITERAL))
    {
        return parser_parse_float(parser);
    }
    else if (parser_accept(parser, ATOM))
    {
        return parser_parse_atom(parser);
    }
    else if (parser_accept(parser, QUOTE))
    {
        return parser_parse_quoted_expression(parser);
    }
    else
    {
        assert(false);
        // TODO: error
    }
}

Expression*
parser_parse_program(Parser* parser)
{
    ExpressionListBuilder list_builder = expression_list_builder_new();

    while (!parser_accept(parser, END_OF_INPUT))
    {
        expression_list_builder_add(
            &list_builder,
            parser_parse_expression(parser)
        );
    }

    return expression_new(
        PROGRAM_EXPR,
        (union ExpressionValue){
            .expression_list = expression_list_builder_build_node(&list_builder)
        }
    );
}

void
parser_free(Parser* parser)
{
    SAFE_FREE(parser->previous_token);
    SAFE_FREE(parser->current_token);
    SAFE_FREE(parser->next_token);
}

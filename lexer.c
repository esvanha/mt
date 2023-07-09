#include "util.h"
#include "lexer.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

//void
//token_stack_free(TokenListNode* head_node)
//{
//    while (head_node != NULL)
//    {
//        TokenListNode* new_head = head_node->next_node;
//        SAFE_FREE(head_node);
//        head_node = new_head;
//    }
//}

void
token_print(const Token* token)
{
    fputs("Token(", stdout);

    switch (token->token_type)
    {
        case IDENTIFIER:
            printf("IDENTIFIER, %s)\n", token->value.str_value);
            break;
        case L_PAREN:
            puts("L_PAREN)");
            break;
        case R_PAREN:
            puts("R_PAREN)");
            break;
        case INTEGER_LITERAL:
            printf("INTEGER_LITERAL, %d)\n", token->value.int_value);
            break;
        case FLOAT_LITERAL:
            //.. Explicitly cast to double to surpress double promotion
            //   warning.
            printf("FLOAT_LITERAL, %f)\n", (double)token->value.float_value);
            break;
        case ATOM:
            printf("ATOM, %s)\n", token->value.str_value);
            break;
        case QUOTE:
            puts("QUOTE)");
            break;
        default:
            //.. All cases should be implemented, making this part unreachable.
            assert(false);
    }
}

//void
//token_stack_dbg(TokenListNode* node)
//{
//    while (node != NULL)
//    {
//        token_print(node->token);
//
//        node = node->next_node;
//    }
//}

Lexer
lexer_new(const char* input)
{
    return (Lexer){
        .input = input,
        .line_count = 1,
        .column_count = 0
    };
}

static bool
is_identifier_character(char ch)
{
    return (ch >= 'A' && ch <= 'z') || ch == '+' || ch == '-';
}

static bool
is_whitespace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

static bool
is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

static bool
is_atom_character(char ch)
{
    return (ch >= 'A' && ch <= 'z') || ch == '-' || is_digit(ch);
}

static void
lexer_skip_whitespace(Lexer *lexer)
{
    while (is_whitespace(*lexer->input))
    {
        lexer->input++;
    }
}

static bool
is_token_ending(char ch)
{
    return is_whitespace(ch) || ch == '(' || ch == ')';
}

static bool
lexer_take_identifier(Lexer *lexer, char** identifier)
{
    const char* original_input = lexer->input;
    int size = 0;
    char ch;

    while (!is_token_ending(ch = *lexer->input))
    {
        if (!is_identifier_character(ch))
        {
            lexer->input = original_input;
            return false;
        }

        size++;
        lexer->input++;
    }

    *identifier = malloc((size + 1) * sizeof(char));
    if (*identifier == NULL)
    {
        fputs("Couldn't allocate bytes", stderr);
        exit(EXIT_FAILURE);
    }

    strncpy(*identifier, original_input, size);
    (*identifier)[size] = '\0';

    return true;
}

static char*
lexer_take_atom(Lexer *lexer)
{
    const char* original_input = lexer->input;
    char ch;
    int size = 0;

    if (*lexer->input++ != ':')
    {
        //.. This function should only be called when a ':' is encountered.
        assert(false);
    }

    while (is_atom_character(ch = *lexer->input))
    {
        size++;
        lexer->input++;
    }

    char* atom = malloc((size + 1) * sizeof(char));
    if (atom == NULL)
    {
        assert(false);
        // TODO: error returnen
    }

    strncpy(atom, original_input + 1, size);
    atom[size] = '\0';

    return atom;
}

static bool
lexer_take_integer(Lexer *lexer, int* int_value)
{
    const char* original_input = lexer->input;
    *int_value = 0;

    while (!is_token_ending(*lexer->input)) 
    {
        if (!is_digit(*lexer->input))
        {
            lexer->input = original_input;
            return false;
        }

        *int_value *= 10;
        *int_value += *lexer->input - '0';

        lexer->input++;
    }

    return true;
}

static bool
lexer_take_float(Lexer* lexer, float* float_value)
{
    const char* original_input = lexer->input;
    bool reached_decimals = false;
    int decimals_size = 1;
    int decimals = 0;
    *float_value = 0.0;

    while (!is_token_ending(*lexer->input))
    {
        if (*lexer->input == '.')
        {
            if (reached_decimals)
            {
                lexer->input = original_input;
                return false;
            }
            else
            {
                reached_decimals = true;
            }

            lexer->input++;
            continue;
        }

        if (!is_digit(*lexer->input))
        {
            lexer->input = original_input;
            return false;
        }

        const int digit = *lexer->input - '0';

        if (reached_decimals)
        {
            decimals_size *= 10;
            decimals *= 10;
            decimals += digit;
        }
        else
        {
            *float_value *= 10;
            *float_value += digit;
        }

        lexer->input++;
    }

    *float_value += (float)decimals / decimals_size;

    return true;
}

void
token_free(Token* token)
{
    if (token->token_type == IDENTIFIER || token->token_type == ATOM)
    {
        SAFE_FREE(token->value.str_value);
    }
}

Token
lexer_next_token(Lexer* lexer)
{
    lexer_skip_whitespace(lexer);

    if (*lexer->input == '\0') 
    {
        lexer->input++;
        return (Token) { .token_type = END_OF_INPUT };
    }
    else if (*lexer->input == '(')
    {
        lexer->input++;
        return (Token) { .token_type = L_PAREN };
    }
    else if (*lexer->input == ')')
    {
        lexer->input++;
        return (Token) { .token_type = R_PAREN };
    }
    else if (*lexer->input == '\'')
    {
        lexer->input++;
        return (Token) { .token_type = QUOTE };
    }
    else if (*lexer->input == ':')
    {
        return (Token) {
            .token_type = ATOM,
            .value.str_value = lexer_take_atom(lexer)
        };
    }
    else if (is_identifier_character(*lexer->input))
    {
        char* identifier;
        if (lexer_take_identifier(lexer, &identifier))
        {
            return (Token){
                .token_type = IDENTIFIER,
                .value.str_value = identifier
            };
        }
    }
    else if (is_digit(*lexer->input))
    {
        int int_value;
        float float_value;

        if (lexer_take_integer(lexer, &int_value))
        {
            return (Token){
                .token_type = INTEGER_LITERAL,
                .value.int_value = int_value
            };
        }
        else if (lexer_take_float(lexer, &float_value))
        {
            return (Token){
                .token_type = FLOAT_LITERAL,
                .value.float_value = float_value
            };
        }
    }
    
    fprintf(
        stderr,
        "Unexpected character at line %d, column %d: `%c` in remaining input "
            "`%s`\n",
        lexer->line_count,
        lexer->column_count,
        *lexer->input,
        lexer->input
    );
    exit(EXIT_FAILURE);
}

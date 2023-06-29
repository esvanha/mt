#include "expression.h"
#include "util.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

Expression* expression_new(
    enum ExpressionType type,
    union ExpressionValue value
) {
    Expression* expression = malloc(sizeof(Expression));
    if (expression == NULL)
    {
        // TODO: error returnen
        assert(false);
    }

    expression->expression_type = type;
    expression->value = value;

    return expression;
}

void
expression_type_print(enum ExpressionType expression_type)
{
    switch (expression_type)
    {
        case LIST_EXPR:
            fputs("LIST_EXPR", stdout);
            break;
        case INTEGER_EXPR:
            fputs("INTEGER_EXPR", stdout);
            break;
        case FLOAT_EXPR:
            fputs("FLOAT_EXPR", stdout);
            break;
        case ATOM_EXPR:
            fputs("ATOM_EXPR", stdout);
            break;
        case IDENTIFIER_EXPR:
            fputs("IDENTIFIER_EXPR", stdout);
            break;
        case QUOTED_EXPR:
            fputs("QUOTED_EXPR", stdout);
            break;
        default:
            // TODO: comment
            assert(false);
    }
}

void
expression_print(Expression* expression)
{
    printf("Expression(");

    expression_type_print(expression->expression_type);

    printf(", ");

    switch (expression->expression_type)
    {
        case LIST_EXPR:
            fputc('(', stdout);

            ExpressionListNode* current_node = expression->value.expression_list;
            while (current_node != NULL)
            {
                expression_print(current_node->expression);

                current_node = current_node->next;
            }

            fputc(')', stdout);
            break;
        case INTEGER_EXPR:
            printf("%d", expression->value.int_value);
            break;
        case FLOAT_EXPR:
            printf("%f", expression->value.float_value);
            break;
        case ATOM_EXPR:
            printf(":%s", expression->value.str_value);
            break;
        case IDENTIFIER_EXPR:
            printf("%s", expression->value.str_value);
            break;
        case QUOTED_EXPR:
            expression_print(expression->value.quoted_expression);
            break;
        default:
            // TODO: comment
            assert(false);   
    }
    
    fputc(')', stdout);
}

void
expression_free(Expression* expression)
{
    switch (expression->expression_type)
    {
        case LIST_EXPR:
            expression_list_node_free(expression->value.expression_list);
            break;
        case IDENTIFIER_EXPR:
        case ATOM_EXPR:
            SAFE_FREE(expression->value.str_value);
            break;
        default:
            //.. Nothing to be freed
    }

    SAFE_FREE(expression);
}

void
expression_list_node_insert(
    ExpressionListNode* node,
    Expression* expression
) {
    //.. Skip to end
    while ((node = node->next) != NULL) {}

    node->next = malloc(sizeof(ExpressionListNode));
    if (node->next == NULL)
    {
        // TODO: error returnen
        return;
    }

    node->next->expression = expression;
    node->next->next = NULL;
}

void
expression_list_node_free(ExpressionListNode* parent)
{
    while ((parent = parent->next) != NULL)
    {
        expression_free(parent->expression);
        SAFE_FREE(parent);
    }
}

static Expression*
expression_evaluate_list(Expression* expression)
{
    ExpressionListNode* parent = expression->value.expression_list;

    if (parent == NULL)
    {
        // TODO: wat te doen hier?
        assert(false);
    }

    // TODO: eigk moet dit geevaluated worden
    if (parent->expression->expression_type != IDENTIFIER_EXPR)
    {
        // TODO: wat te doen hier?
        assert(false);
    }

    if (strcmp(parent->expression->value.str_value, "let") == 0)
    {
        puts("let called");
    }

    ExpressionListNode* remainder = parent->next;

    // TODO: rest implementeren
    assert(false);
}

static Expression*
expression_evaluate_identifier(Expression* expression)
{
    // TODO
    assert(false);
}

Expression*
expression_evaluate(Expression* expression)
{
    switch (expression->expression_type)
    {
        case LIST_EXPR:
            return expression_evaluate_list(expression);
        case INTEGER_EXPR:
            return expression;
        case FLOAT_EXPR:
            return expression;
        case ATOM_EXPR:
            return expression;
        case IDENTIFIER_EXPR:
            return expression_evaluate_identifier(expression);
        case QUOTED_EXPR:
            return expression;
        default:
            // TODO: comment
            assert(false);
    }
}

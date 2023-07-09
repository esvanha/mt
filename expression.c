#include "expression.h"
#include "util.h"
#include "notes.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

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
    while (parent != NULL)
    {
        expression_free(parent->expression);
        ExpressionListNode* new_parent = parent->next;
        SAFE_FREE(parent);
        parent = new_parent;
    }
}

static Expression*
expression_evaluate_list(
    Expression* expression,
    EvaluationContext* evaluation_context
) {
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

    if (strcmp(parent->expression->value.str_value, "sleep") == 0)
    {
        puts("yas");
        sleep(parent->next->expression->value.int_value);
        puts("huh");
        return NULL;
    }

    if (strcmp(parent->expression->value.str_value, "play") != 0)
    {
        return NULL;
//        puts("let called");
    }

    ExpressionListNode* remainder = parent->next;
    char* atom_value = remainder->expression->value.str_value;

    // TODO: temporary testing code
    float hz = 0;
    if (strcmp(atom_value, "C4") == 0)
    {
        hz = NOTE_C4;
    }
    else if (strcmp(atom_value, "D4") == 0)
    {
        hz = NOTE_D4;
    }
    else if (strcmp(atom_value, "B4") == 0)
    {
        hz = NOTE_B4;
    }
    else
    {
        return NULL;
    }

    event_bus_notify(
        evaluation_context->event_bus,
        (EventMessage){
            .type = EVENT_NOTE_ENABLE,
            .event.note_enable = (EventNoteEnable){
                .amplitude = 0.8f,
                .hz = hz
            }
        }
    );

    return NULL;
    // TODO: rest implementeren
    //assert(false);
}

static Expression*
expression_evaluate_identifier(
    Expression* expression,
    EvaluationContext* evaluation_context
) {
    // TODO
    assert(false);
}

static Expression*
expression_evaluate_program(
    Expression* expression,
    EvaluationContext* evaluation_context
) {
    ExpressionListNode* parent_node = expression->value.expression_list;
    while (parent_node != NULL)
    {
        expression_evaluate(parent_node->expression, evaluation_context);

        ExpressionListNode* old_parent = parent_node;
        parent_node = parent_node->next;
        SAFE_FREE(old_parent);
    }

    // TODO: empty list returnen?
    return NULL;
}

// TODO: die synth verder uitbreiden (ASDR laten werken)
// TODO: eventbus maken
// TODO: tijdens evaluation messsages op eventbus zetten
// TODO: synth deze messages laten pollen
Expression*
expression_evaluate(
    Expression* expression,
    EvaluationContext* evaluation_context
) {
    switch (expression->expression_type)
    {
        case LIST_EXPR:
            return expression_evaluate_list(expression, evaluation_context);
        case INTEGER_EXPR:
            return expression;
        case FLOAT_EXPR:
            return expression;
        case ATOM_EXPR:
            return expression;
        case IDENTIFIER_EXPR:
            return expression_evaluate_identifier(
                expression, evaluation_context
            );
        case QUOTED_EXPR:
            return expression;
        case PROGRAM_EXPR:
            return expression_evaluate_program(expression, evaluation_context);
        default:
            // TODO: comment
            assert(false);
    }
}

EvaluationContext
evaluation_context_new(EventBus* event_bus)
{
    return (EvaluationContext){ .event_bus = event_bus };
}

ExpressionListBuilder
expression_list_builder_new()
{
    return (ExpressionListBuilder){
        .current_node = NULL,
        .parent_node = NULL
    };
}

void
expression_list_builder_add(
    ExpressionListBuilder* builder,
    Expression* expression
)
{
    ExpressionListNode* new_node = malloc(sizeof(ExpressionListBuilder));
    if (new_node == NULL)
    {
        // TODO: error returnen
        assert(false);
    }

    new_node->expression = expression;
    new_node->next = NULL;

    if (builder->parent_node == NULL && builder->current_node == NULL)
    {
        builder->parent_node = new_node;
        builder->current_node = builder->parent_node;
    }
    else
    {
        builder->current_node->next = new_node;
        builder->current_node = new_node;
    }
}

ExpressionListNode*
expression_list_builder_build_node(ExpressionListBuilder* builder)
{
    return builder->parent_node;
}

Expression*
expression_list_builder_build_expr(ExpressionListBuilder* builder)
{
    return expression_new(
        LIST_EXPR,
        (union ExpressionValue){ .expression_list = builder->parent_node }
    );
}

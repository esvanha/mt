#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "event_bus.h"

//enum Waveform { SAW };
//
//typedef struct {
//    float hz;
//    float attack, sustain, decay, release;
//    enum Waveform waveform;
//} MTInstrument;
//
//typedef struct {
//
//} MTEnvironment;
typedef struct {
    EventBus* event_bus;
} EvaluationContext;

enum ExpressionType {
    PROGRAM_EXPR, LIST_EXPR, INTEGER_EXPR, FLOAT_EXPR, ATOM_EXPR,
    IDENTIFIER_EXPR, QUOTED_EXPR
};

struct ExpressionListNode;

union ExpressionValue {
    int int_value;
    float float_value;
    char* str_value;
    struct ExpressionListNode* expression_list;
    struct Expression* quoted_expression;
};

typedef struct Expression {
    enum ExpressionType expression_type;
    union ExpressionValue value;
} Expression;

typedef struct ExpressionListNode {
    struct ExpressionListNode* next;
    Expression* expression;
} ExpressionListNode;

Expression* expression_new(
    enum ExpressionType type,
    union ExpressionValue value
);
void expression_free(Expression* expression);
void expression_list_node_insert(
    ExpressionListNode* node,
    Expression* expression
);
void expression_list_node_free(ExpressionListNode* parent);

void expression_print(Expression* expression);
Expression* expression_evaluate(
    Expression* expression,
    EvaluationContext* evaluation_context
);

EvaluationContext evaluation_context_new(EventBus* event_bus);

/**
 * ExpressionListBuilder
 */
typedef struct {
    ExpressionListNode* parent_node;
    ExpressionListNode* current_node;
} ExpressionListBuilder;

ExpressionListBuilder expression_list_builder_new();
void expression_list_builder_add(
    ExpressionListBuilder* builder,
    Expression* expression
);
ExpressionListNode* expression_list_builder_build_node(
    ExpressionListBuilder* builder
);
Expression* expression_list_builder_build_expr(ExpressionListBuilder* builder);

#endif

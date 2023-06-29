#ifndef EXPRESSION_H
#define EXPRESSION_H

enum Waveform { SAW };

typedef struct {
    float hz;
    float attack, sustain, decay, release;
    enum Waveform waveform;
} MTInstrument;

typedef struct {

} MTEnvironment;

enum ExpressionType {
    LIST_EXPR, INTEGER_EXPR, FLOAT_EXPR, ATOM_EXPR, IDENTIFIER_EXPR, QUOTED_EXPR
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
    //union {
    //    int int_value;
    //    float float_value;
    //    char* str_value;
    //    struct ExpressionListNode* expression_list;
    //    struct Expression* quoted_expression;
    //} value;
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
Expression* expression_evaluate(Expression* expression);

#endif

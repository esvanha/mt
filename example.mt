(let :basic-synth
    (make-instrument
        :hz 22300
        :attack 0.3
        :sustain 0.4
        :decay 0.2
        :release 0.1
        :type :saw
    )
)

(each :note '(:A4 :A5 :A6 :A7) (
    (play basic-synth note)
))
(play :basic-synth :A5)

/*

Expression {
    .expression_type = LIST_EXPR,
    .value = ExpressionList[
        Expression { .expression_type = IDENTIFIER_EXPR, .value = "let" },
        Expression { .expression_type = ATOM_EXPR, .value = "basic-synth" },
        Expression {
            .expression_type = LIST_EXPR,
            .value = ExpressionList[
                Expression { .expression_type = IDENTIFIER_EXPR, .value = "make-instrument" },
                Expression { .expression_type = ATOM_EXPR, .value = "hz" },
                Expression { .expression_type = INTEGER_EXPR, .value = 22300 },
                Expression { .expression_type = ATOM_EXPR, .value = "attack" },
                Expression { .expression_type = FLOAT_EXPR, .value = 0.3 },
            ]
        }
    ]
}

*/
#include <stdio.h>
#include "lexer.h"
#include "expression.h"
#include "parser.h"
#include "util.h"
#include <raylib.h>
#include <unistd.h>
#include "audio_system.h"
#include "event_bus.h"
#include "notes.h"
#include <assert.h>

/*
 * instrument = adsr + oscillator(s) + effects
 * NoteEnable<hz, instrument, time (ms), amplitude>
 * NoteDisable<hz, instrument, time (ms), amplitude>
 * NoteFinite<time: ms, hz, instrument, amplitude>
 */

int
main(int argc, char* argv[])
{
    EventBus event_bus = event_bus_new();

    audio_system_initialize(44100, &event_bus);

    char* str = "\
        (let :basic-synth\
            (make-instrument\
                :attack 0.345\
                :sustain 0.456\
                :decay 0.2\
                :release 0.123\
                :type :saw\
            )\
        )\
        \
        (looper :looper-a (\
            (each :note '(:A4 :A5 :A6 :A7) (\
                (play basic-synth note)\
            ))\
            (play :basic-synth :A5)\
        ))\
        (play-loop :looper-a)\
    ";

    // TODO: aan lexer toevoegn dat er geen whitespace voor EOF hoeft te staan
    Lexer lexer = lexer_new("(play :C3) "); //str);
    Parser parser = parser_new(&lexer);

    audio_system_start();

    Expression* expression = parser_parse_expression(&parser);
    EvaluationContext context = evaluation_context_new(&event_bus);
    expression_evaluate(expression, &context);

    sleep(5);

    audio_system_stop();
    parser_free(&parser);
    event_bus_free(&event_bus);

    return EXIT_SUCCESS;
}

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
main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[])
{
    EventBus event_bus = event_bus_new();

    audio_system_initialize(44100, &event_bus);

/*    char* str = "\
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
    ";*/
    char* str_b = "\
        (play :D4)\
        (sleep 1)\
        (play :C4)\
        (sleep 2)\
        (play :B4)\
        (sleep 1)\
        (play :C4)\
        (sleep 1)\
    ";

    // TODO: aan lexer toevoegn dat er geen whitespace voor EOF hoeft te staan
    Lexer lexer = lexer_new(str_b); //str);
    Parser parser = parser_new(&lexer);

    audio_system_start();

    InitWindow(200, 200, "");
    SetTargetFPS(60);

//    Expression* expression = parser_parse_program(&parser);
//    EvaluationContext context = evaluation_context_new(&event_bus);
//    expression_evaluate(expression, &context);

    int a_was_pressed = 0;
    int b_was_pressed = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing(); EndDrawing();
        
        if (IsKeyPressed(KEY_A) && !a_was_pressed)
        {
            a_was_pressed = 1;

            event_bus_notify(&event_bus, (EventMessage){
                .type = EVENT_NOTE_ENABLE,
                .event.note_enable = (EventNoteEnable){
                    .amplitude = 0.8f,
                    .hz = NOTE_A4
                }
            });
        }
        if (IsKeyPressed(KEY_B) && !b_was_pressed)
        {
            b_was_pressed = 1;

            event_bus_notify(&event_bus, (EventMessage){
                .type = EVENT_NOTE_ENABLE,
                .event.note_enable = (EventNoteEnable){
                    .amplitude = 0.8f,
                    .hz = NOTE_B4
                }
            });
        }

        if (IsKeyReleased(KEY_A))
        {
            a_was_pressed = 0;

            event_bus_notify(&event_bus, (EventMessage){
                .type = EVENT_NOTE_DISABLE,
                .event.note_disable = (EventNoteDisable){
                    .hz = NOTE_A4
                }
            });
        }
        if (IsKeyReleased(KEY_B))
        {
            b_was_pressed = 0;

            event_bus_notify(&event_bus, (EventMessage){
                .type = EVENT_NOTE_DISABLE,
                .event.note_disable = (EventNoteDisable){
                    .hz = NOTE_B4
                }
            });
        }
    }

    audio_system_stop();
    parser_free(&parser);
    event_bus_free(&event_bus);

    return EXIT_SUCCESS;
}

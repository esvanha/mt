#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>
#include "notes.h"
#include "asdr_envelope.h"
#include "lexer.h"
#include "expression.h"
#include "parser.h"

#define SAFE_FREE(x) do { free(x); x = NULL; } while (0)

const float M_PI = 3.14;

static void audio_callback(void*, uint8_t*, int);
static inline double hz_to_angular_velocity(double hertz);

float note = 0;
int oscillator_type = 0;
ASDREnvelope* asdr_envelope = NULL;
struct timeval start_time;

static double
elapsed_time_ms()
{
    struct timeval stop_time;

    gettimeofday(&stop_time, NULL);

    return (
        (stop_time.tv_sec - start_time.tv_sec) * 1000000
        + stop_time.tv_usec - start_time.tv_usec
    ) / 1000000.0;
}

int
main(int argc, char* argv[])
{
    char* str = "\
        (let :basic-synth\
            (make-instrument\
                :hz 22300\
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

#if false
    ExpressionListNode* node = malloc(sizeof(ExpressionListNode));
    node->expression = malloc(sizeof(Expression));
    node->expression->expression_type = IDENTIFIER_EXPR;
    node->expression->value.str_value = "let";
    node->next = NULL;

    Expression list = (Expression){
        .expression_type = LIST_EXPR,
        .value.expression_list = node
    };

    expression_evaluate(&list);

    expression_list_node_free(node);

    return EXIT_SUCCESS;
#endif

    // TODO: aan lexer toevoegn dat er geen whitespace voor EOF hoeft te staan
    Lexer lexer = lexer_new(str);
    Parser parser = parser_new(&lexer);

    expression_print(parser_parse_expression(&parser));

    //Token token;
    //while ((token = lexer_next_token(&lexer)).token_type != END_OF_INPUT)
    //{
    //    token_print(&token);
    //    token_free(&token);
    //}

    parser_free(&parser);

    return 0;


    gettimeofday(&start_time, NULL);

    uint64_t samples_played = 0;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(
            stderr,
            "Error occured while initializing SDL's audio: %s\n",
            SDL_GetError()
        );
        return EXIT_FAILURE;
    }

    SDL_AudioSpec audio_spec_want, audio_spec;

    SDL_zero(audio_spec_want);

    audio_spec_want.freq = 44100;
    audio_spec_want.format = AUDIO_F32;
    audio_spec_want.channels = 1; //.. Mono for now
    audio_spec_want.samples = 512; //.. Buffer size
    audio_spec_want.callback = audio_callback;
    audio_spec_want.userdata = (void*)&samples_played;

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
        NULL, 0, &audio_spec_want, &audio_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE
    );

    if (!audio_device_id) {
        fprintf(
            stderr,
            "Error occured while creating SDL audio device: %s\n",
            SDL_GetError()
        );
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "mt studio v0.1",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(
            stderr,
            "Error occured while creating SDL window: %s\n",
            SDL_GetError()
        );
        SDL_CloseAudioDevice(audio_device_id);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_ShowWindow(window);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);

    //.. Unpause audio device
    SDL_PauseAudioDevice(audio_device_id, 0);

    asdr_envelope = asdr_envelope_new();

    bool running = true;
    while (running) {
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event) != 0) {
            if (sdl_event.type == SDL_QUIT) {
                running = false;
            } else if (sdl_event.type == SDL_KEYDOWN) {
                switch (sdl_event.key.keysym.sym) {
                    case SDLK_a:
                        asdr_envelope_enable_note(asdr_envelope, elapsed_time_ms());
                        note = NOTE_C5;
                        break;
                    case SDLK_w: note = NOTE_CS5; break;
                    case SDLK_s: note = NOTE_D5; break;
                    case SDLK_e: note = NOTE_DS5; break;
                    case SDLK_f: note = NOTE_E5; break;
                    case SDLK_g: note = NOTE_F5; break;
                    case SDLK_y: note = NOTE_FS5; break;
                    case SDLK_h: note = NOTE_G5; break;
                    case SDLK_u: note = NOTE_GS5; break;
                    case SDLK_j: note = NOTE_A5; break;
                    case SDLK_i: note = NOTE_AS5; break;
                    case SDLK_k: note = NOTE_B5; break;
                    case SDLK_UP:
                        oscillator_type = (oscillator_type + 1) % 6;
                        break;
                }
            } else if (sdl_event.type == SDL_KEYUP) {
                asdr_envelope_disable_note(asdr_envelope, elapsed_time_ms());
                //note = 0;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();

    SAFE_FREE(asdr_envelope);

    return EXIT_SUCCESS;
}

static inline double
hz_to_angular_velocity(double hertz)
{
    return hertz * 2.0 * M_PI;
}

static double
oscillator(double hz, double time, int type)
{
    switch (type)
    {
        case 0: //.. Sine wave
            return sin(hz_to_angular_velocity(hz) * time);
        case 1: //.. Square wave
            return sin(hz_to_angular_velocity(hz) * time) > 0.0 ? 1.0 : -1.0;
        case 2: //.. Triangle wave
            return asin(sin(hz_to_angular_velocity(hz) * time)) * 2.0 / M_PI;
        case 3: //.. Saw wave (slow)
            double output = 0.0;

            for (double n = 1.0; n < 100.0; n++)
                output += (sin(n * hz_to_angular_velocity(hz) * time)) / n;

            return output * (2.0 / M_PI);
        case 4: //.. Saw wave (optimized)
            return (2.0 / M_PI) * (hz * M_PI * fmod(time, 1.0 / hz) - (M_PI / 2.0));
        case 5: //.. Pseudo-random noise
            return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;
        default:
            return 0.0;
    }
}

static void
audio_callback(void* userdata, uint8_t* stream, int len)
{
    uint64_t* samples_played = (uint64_t*)userdata;
    float* fstream = (float*)stream;

    //static float amplitude = 0.2;
//    static float frequency = NOTE_A5;//200.0;

    for (int sid = 0; sid < len / 8; ++sid)
    {
        double time = (*samples_played + sid) / 44100.0;
        double x = hz_to_angular_velocity(note);

        float amplitude = (float)asdr_envelope_amplitude(asdr_envelope, elapsed_time_ms());//time);
        printf("Amplitude: %f\n", amplitude);

//        fstream[1 * sid] = amplitude * oscillator(x, time, oscillator_type);//sin(x);
        fstream[2 * sid] = amplitude * oscillator(x, time, oscillator_type);//sin(x);
    }
    // https://www.youtube.com/watch?v=OSCzKOqtgcA

    *samples_played += len / 8;
}

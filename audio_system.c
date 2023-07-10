#include "audio_system.h"
#include "notes.h"
#include "util.h"
#include "adsr_envelope.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

static void callback(void* buffer, unsigned int frames);

// Since there's no way to pass additional data to the callback function, we
// have to resort to a static state.
static AudioSystem* audio_system = NULL;

double init_clock_ticks = 0.0;

float active_hz = 0.0;

static double
seconds_passed(void)
{
    return (((double)clock()) - init_clock_ticks) / CLOCKS_PER_SEC;
}

void
audio_system_initialize(int sample_rate, EventBus* event_bus)
{
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(sample_rate);//1024);

    audio_system = malloc(sizeof(AudioSystem));
    if (audio_system == NULL)
    {
        // TODO: betere error
        assert(false);
    }
    audio_system->sample_rate = sample_rate;
    audio_system->stream = LoadAudioStream(
        /* Sample rate */ sample_rate,
        /* Sample size */ 8 * sizeof(float),
        /* Channels (1 for mono) */ 1
    );
    audio_system->event_bus = event_bus;

    init_clock_ticks = clock();

    SetAudioStreamCallback(audio_system->stream, callback);
}

void
audio_system_start(void)
{
    PlayAudioStream(audio_system->stream);
}

void
audio_system_stop(void)
{
    StopAudioStream(audio_system->stream);
    UnloadAudioStream(audio_system->stream);
    CloseAudioDevice();
}

static float phase_a = 0.0f;
static float phase_b = 0.0f;
static ADSREnvelope* adsr_envelope_a = NULL;
static ADSREnvelope* adsr_envelope_b = NULL;
float note_a = 0.0f;
float note_b = 0.0f; 

static void
callback(void* buffer, unsigned int frames)
{
    EventMessage* event_message = NULL;
    float passed_seconds = (float)seconds_passed();

    if (event_bus_poll(audio_system->event_bus, &event_message))
    {
        switch (event_message->type)
        {
            case EVENT_NOTE_ENABLE:
                if (adsr_envelope_a == NULL)
                {
                    note_a = event_message->event.note_enable.hz;
                    adsr_envelope_a = adsr_envelope_new(0.1f, 0.05f, 0.08f, 0.5f, 0.6f);
                    adsr_envelope_enable_note(adsr_envelope_a, passed_seconds);
                }
                else if (adsr_envelope_b == NULL)
                {
                    //}
                    note_b = event_message->event.note_enable.hz;
                    adsr_envelope_b = adsr_envelope_new(0.1f, 0.05f, 0.08f, 0.7f, 0.9f);
                    adsr_envelope_enable_note(adsr_envelope_b, passed_seconds);
                }
                break;
            case EVENT_NOTE_DISABLE:
                if ((int)event_message->event.note_disable.hz == (int)note_a)
                {
                    puts("note a");
                    //note_a = 0.0f;
                    assert(adsr_envelope_a != NULL);
                    adsr_envelope_disable_note(adsr_envelope_a, passed_seconds);
                }
                else if ((int)event_message->event.note_disable.hz == (int)note_b)
                {
                    puts("note b");
                    //note_b = 0.0f;
                    assert(adsr_envelope_b != NULL);
                    adsr_envelope_disable_note(adsr_envelope_b, passed_seconds);
                }
                else
                {
                    puts("else");
                }
                break;
            default:
                break;
        }

        SAFE_FREE(event_message);
    }

    float* d = (float*)buffer;

    memset(d, 0, frames);

//    if (adsr_envelope_a == NULL)
//    {
//        memset(d, 0, frames);
//        return;
//    }

    if (adsr_envelope_a != NULL && adsr_envelope_finished(adsr_envelope_a, passed_seconds))
        SAFE_FREE(adsr_envelope_a);
    if (adsr_envelope_b != NULL && adsr_envelope_finished(adsr_envelope_b, passed_seconds))
        SAFE_FREE(adsr_envelope_b);

    float phase_delta_a = note_a / 44100.0f;
    float phase_delta_b = note_b / 44100.0f;

    const float ampl_a =
        (adsr_envelope_a == NULL)
        ? 0.0f
        : (float)adsr_envelope_amplitude(adsr_envelope_a, passed_seconds);
    const float ampl_b =
        (adsr_envelope_b == NULL)
        ? 0.0f
        : (float)adsr_envelope_amplitude(adsr_envelope_b, passed_seconds);

    for (unsigned int i = 0; i < frames; i++)
    {
        d[i] += ampl_a * sinf(2 * PI * phase_a);
        d[i] += ampl_b * sinf(2 * PI * phase_b);

        phase_a += phase_delta_a;
        if (phase_a > 1.0f)
        {
            phase_a -= 1.0f;
        }

        phase_b += phase_delta_b;
        if (phase_b > 1.0f)
        {
            phase_b -= 1.0f;
        }
    }
}

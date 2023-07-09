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

//static double
//oscillator(double hz, double time)
//{
//    return sin(hz * time);
//}

float phase = 0.0f;
ADSREnvelope* adsr_envelope = NULL;

static void
callback(void* buffer, unsigned int frames)
{
    EventMessage* event_message = NULL;

    if (event_bus_poll(audio_system->event_bus, &event_message))
    {
        switch (event_message->type)
        {
            case EVENT_NOTE_ENABLE:
                active_hz = event_message->event.note_enable.hz;
                SAFE_FREE(adsr_envelope);
                adsr_envelope = adsr_envelope_new();
                adsr_envelope_enable_note(adsr_envelope, (float)seconds_passed());
                break;
            case EVENT_NOTE_DISABLE:
                assert(adsr_envelope != NULL);
                adsr_envelope_disable_note(adsr_envelope, (float)seconds_passed());
                break;
        }

        SAFE_FREE(event_message);
    }

    //if (adsr_envelope == NULL)
    //    return;

    float* d = (float*)buffer;
    float phase_delta = active_hz / 44100.0f;

    for (unsigned int i = 0; i < frames; i++)
    {
        if (adsr_envelope == NULL)
        {
            d[i] = 0.0f;
        }
        else
        {
//            seconds_passed();
            float ampl = (float)adsr_envelope_amplitude(adsr_envelope, (float)seconds_passed());
            d[i] = ampl * sinf(2 * PI * phase);
 //           printf("AMPLITUDE: %f\n", ampl);
        }
 //       printf("d[%d] = %f\n", i, d[i]);

        phase += phase_delta;
        if (phase > 1.0f)
        {
            phase -= 1.0f;
        }
    }
}

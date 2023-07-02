#include "audio_system.h"
#include "notes.h"
#include "util.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static void callback(void* buffer, unsigned int frames);

// Since there's no way to pass additional data to the callback function, we
// have to resort to a static state.
static AudioSystem* audio_system;

float active_hz = 0.0;

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

    SetAudioStreamCallback(audio_system->stream, callback);
}

void
audio_system_start()
{
    PlayAudioStream(audio_system->stream);

    
}

void
audio_system_stop()
{
    StopAudioStream(audio_system->stream);
    UnloadAudioStream(audio_system->stream);
    CloseAudioDevice();
}

static inline double
hz_to_angular_velocity(double hertz)
{
    return hertz * 2.0 * PI;
}

static double
oscillator(double hz, double time)
{
    return sin(hz * time);
}

float phase = 0.0;

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
                break;
        }

        SAFE_FREE(event_message);
    }

    float* d = (float*)buffer;
    float phase_delta = active_hz / 44100.0f;

    for (unsigned int i = 0; i < frames; i++)
    {
        d[i] = 0.2 * sinf(2 * PI * phase);

        phase += phase_delta;
        if (phase > 1.0)
        {
            phase -= 1.0;
        }
    }
}

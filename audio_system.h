#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "event_bus.h"
#include <raylib.h>

typedef struct {
    int sample_rate;
    AudioStream stream;
    float phase;
    int stream_buffer_size;
    EventBus* event_bus;
} AudioSystem;

void audio_system_initialize(int sample_rate, EventBus* event_bus);
void audio_system_start(void);
void audio_system_stop(void);

typedef struct {
    void (*oscillator_fn)(float* wave_buffer, void* user_data);
    void* user_data;
} OscillatorCallback;

#endif

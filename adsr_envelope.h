#ifndef ADSR_ENVELOPE_H
#define ADSR_ENVELOPE_H

#include <stdbool.h>

typedef struct {
    float attack_time_s;
    float decay_time_s;
    float release_time_s;

    float sustain_amplitude;
    float start_amplitude;

    float trigger_on_time_s;
    float trigger_off_time_s;

    bool note_enabled;
} ADSREnvelope;

ADSREnvelope* adsr_envelope_new();
void adsr_envelope_enable_note(ADSREnvelope* adsr_envelope, float time_on_ms);
void adsr_envelope_disable_note(ADSREnvelope* adsr_envelope, float time_off_ms);
float adsr_envelope_amplitude(ADSREnvelope* adsr_envelope, float time_ms);

#endif

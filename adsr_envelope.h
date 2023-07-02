#ifndef ADSR_ENVELOPE_H
#define ADSR_ENVELOPE_H

#include <stdbool.h>

typedef struct {
    double attack_time_s;
    double decay_time_s;
    double release_time_s;

    double sustain_amplitude;
    double start_amplitude;

    double trigger_on_time_s;
    double trigger_off_time_s;

    bool note_enabled;
} ADSREnvelope;

ADSREnvelope* asdr_envelope_new();
void adsr_envelope_enable_note(ADSREnvelope* adsr_envelope, double time_on_ms);
void adsr_envelope_disable_note(ADSREnvelope* adsr_envelope, double time_off_ms);
double adsr_envelope_amplitude(ADSREnvelope* adsr_envelope, double time_ms);

#endif

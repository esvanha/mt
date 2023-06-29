#ifndef ASDR_ENVELOPE_H
#define ASDR_ENVELOPE_H

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
} ASDREnvelope;

ASDREnvelope* asdr_envelope_new();
void asdr_envelope_enable_note(ASDREnvelope* asdr_envelope, double time_on_ms);
void asdr_envelope_disable_note(ASDREnvelope* asdr_envelope, double time_off_ms);
double asdr_envelope_amplitude(ASDREnvelope* asdr_envelope, double time_ms);

#endif

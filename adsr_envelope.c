#include "adsr_envelope.h"
#include <stdlib.h>
#include <stdio.h>

ADSREnvelope*
adsr_envelope_new()
{
    ADSREnvelope* adsr_envelope = malloc(sizeof(ADSREnvelope));
    if (adsr_envelope == NULL)
    {
        //
    }

    adsr_envelope->attack_time_s = 0.100;
    adsr_envelope->decay_time_s = 0.01;
    adsr_envelope->release_time_s = 0.2;

    adsr_envelope->sustain_amplitude = 0.8;
    adsr_envelope->start_amplitude = 1.0;

    adsr_envelope->trigger_on_time_s = 0.0;
    adsr_envelope->trigger_off_time_s = 0.0;

    adsr_envelope->note_enabled = false;

    return adsr_envelope;
}

void
adsr_envelope_enable_note(ADSREnvelope* adsr_envelope, double time_on_ms)
{
    adsr_envelope->trigger_on_time_s = time_on_ms;
    adsr_envelope->note_enabled = true;
}

void
adsr_envelope_disable_note(ADSREnvelope* adsr_envelope, double time_off_ms)
{
    adsr_envelope->trigger_off_time_s = time_off_ms;
    adsr_envelope->note_enabled = false;
}

double
adsr_envelope_amplitude(ADSREnvelope* adsr_envelope, double time_ms)
{
    double amplitude = 0.0;
    double lifetime_ms = time_ms - adsr_envelope->trigger_on_time_s;
    printf("lifetime: %f\n", lifetime_ms);

    if (adsr_envelope->note_enabled) {
        // Attack
        if (lifetime_ms <= adsr_envelope->attack_time_s)
        {
            puts("attack");
            amplitude = (lifetime_ms / adsr_envelope->attack_time_s) * adsr_envelope->start_amplitude;
            printf("attack amplitude: %f\n", amplitude);
        }

        // Decay
        if (lifetime_ms > adsr_envelope->attack_time_s && lifetime_ms <= (adsr_envelope->attack_time_s + adsr_envelope->decay_time_s))
        {
            puts("decay");
            const double decay_time_progress = (
                (lifetime_ms - adsr_envelope->attack_time_s)
                    / adsr_envelope->decay_time_s
            );
            const double amplitude_gradient = (
                adsr_envelope->sustain_amplitude - adsr_envelope->start_amplitude
            );

            amplitude = (
                decay_time_progress
                * amplitude_gradient
                + adsr_envelope->start_amplitude
            );
        }

        // Sustain
        if (lifetime_ms > (adsr_envelope->attack_time_s + adsr_envelope->decay_time_s))
        {
            puts("sustain");
            amplitude = adsr_envelope->sustain_amplitude;
        }
    } else {
        // Release
        puts("release");
        amplitude = (
            ((time_ms - adsr_envelope->trigger_off_time_s) / adsr_envelope->release_time_s)
            * (0.0 - adsr_envelope->sustain_amplitude)
            + adsr_envelope->sustain_amplitude
        );
    }

    if (amplitude <= 0.0001)
    {
        amplitude = 0;
    }
    
    return amplitude;
}

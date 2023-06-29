#include "asdr_envelope.h"
#include <stdlib.h>
#include <stdio.h>

ASDREnvelope*
asdr_envelope_new()
{
    ASDREnvelope* asdr_envelope = malloc(sizeof(ASDREnvelope));
    if (asdr_envelope == NULL)
    {
        //
    }

    asdr_envelope->attack_time_s = 0.100;
    asdr_envelope->decay_time_s = 0.01;
    asdr_envelope->release_time_s = 0.2;

    asdr_envelope->sustain_amplitude = 0.8;
    asdr_envelope->start_amplitude = 1.0;

    asdr_envelope->trigger_on_time_s = 0.0;
    asdr_envelope->trigger_off_time_s = 0.0;

    asdr_envelope->note_enabled = false;

    return asdr_envelope;
}

void
asdr_envelope_enable_note(ASDREnvelope* asdr_envelope, double time_on_ms)
{
    asdr_envelope->trigger_on_time_s = time_on_ms;
    asdr_envelope->note_enabled = true;
}

void
asdr_envelope_disable_note(ASDREnvelope* asdr_envelope, double time_off_ms)
{
    asdr_envelope->trigger_off_time_s = time_off_ms;
    asdr_envelope->note_enabled = false;
}

double
asdr_envelope_amplitude(ASDREnvelope* asdr_envelope, double time_ms)
{
    double amplitude = 0.0;
    double lifetime_ms = time_ms - asdr_envelope->trigger_on_time_s;
    printf("lifetime: %f\n", lifetime_ms);

    if (asdr_envelope->note_enabled) {
        // Attack
        if (lifetime_ms <= asdr_envelope->attack_time_s)
        {
            puts("attack");
            amplitude = (lifetime_ms / asdr_envelope->attack_time_s) * asdr_envelope->start_amplitude;
            printf("attack amplitude: %f\n", amplitude);
        }

        // Decay
        if (lifetime_ms > asdr_envelope->attack_time_s && lifetime_ms <= (asdr_envelope->attack_time_s + asdr_envelope->decay_time_s))
        {
            puts("decay");
            const double decay_time_progress = (
                (lifetime_ms - asdr_envelope->attack_time_s)
                    / asdr_envelope->decay_time_s
            );
            const double amplitude_gradient = (
                asdr_envelope->sustain_amplitude - asdr_envelope->start_amplitude
            );

            amplitude = (
                decay_time_progress
                * amplitude_gradient
                + asdr_envelope->start_amplitude
            );
        }

        // Sustain
        if (lifetime_ms > (asdr_envelope->attack_time_s + asdr_envelope->decay_time_s))
        {
            puts("sustain");
            amplitude = asdr_envelope->sustain_amplitude;
        }
    } else {
        // Release
        puts("release");
        amplitude = (
            ((time_ms - asdr_envelope->trigger_off_time_s) / asdr_envelope->release_time_s)
            * (0.0 - asdr_envelope->sustain_amplitude)
            + asdr_envelope->sustain_amplitude
        );
    }

    if (amplitude <= 0.0001)
    {
        amplitude = 0;
    }
    
    return amplitude;
}

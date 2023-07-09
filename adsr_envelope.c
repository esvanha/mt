#include "adsr_envelope.h"
#include <stdlib.h>
#include <stdio.h>

/*
 *     |  |    |
 *    /|\ |    |
 *   / | \|____|
 *  /  |  |    |\
 * /   |  |    | \
 *  A   D   S    R 
 */
ADSREnvelope*
adsr_envelope_new(void)
{
    ADSREnvelope* adsr_envelope = malloc(sizeof(ADSREnvelope));
    if (adsr_envelope == NULL)
    {
        //
    }

    adsr_envelope->attack_time_s = 1.0f;//0.100;
    adsr_envelope->decay_time_s = 0.4f;//0.01;
    adsr_envelope->release_time_s = 0.2f;

    adsr_envelope->sustain_amplitude = 0.8f;
    adsr_envelope->start_amplitude = 1.0f;

    adsr_envelope->trigger_on_time_s = 0.0f;
    adsr_envelope->trigger_off_time_s = 0.0f;

    adsr_envelope->note_enabled = false;

    return adsr_envelope;
}

void
adsr_envelope_enable_note(ADSREnvelope* adsr_envelope, float time_on_s)
{
    adsr_envelope->trigger_on_time_s = time_on_s;
    adsr_envelope->note_enabled = true;
}

void
adsr_envelope_disable_note(ADSREnvelope* adsr_envelope, float time_off_s)
{
    adsr_envelope->trigger_off_time_s = time_off_s;
    adsr_envelope->note_enabled = false;
}

float
adsr_envelope_amplitude(ADSREnvelope* adsr_envelope, float time_s)
{
    float amplitude = 0.0f;
    float lifetime_s = time_s - adsr_envelope->trigger_on_time_s;
    //printf("lifetime: %f\n", lifetime_s);

    if (adsr_envelope->note_enabled) {
        // Attack
        if (lifetime_s <= adsr_envelope->attack_time_s)
        {
            //puts("attack");
            amplitude = (lifetime_s / adsr_envelope->attack_time_s) * adsr_envelope->start_amplitude;
            //printf("attack amplitude: %f\n", amplitude);
        }

        // Decay
        if (lifetime_s > adsr_envelope->attack_time_s && lifetime_s <= (adsr_envelope->attack_time_s + adsr_envelope->decay_time_s))
        {
            //puts("decay");
            const float decay_time_progress = (
                (lifetime_s - adsr_envelope->attack_time_s)
                    / adsr_envelope->decay_time_s
            );
            const float amplitude_gradient = (
                adsr_envelope->sustain_amplitude - adsr_envelope->start_amplitude
            );

            amplitude = (
                decay_time_progress
                * amplitude_gradient
                + adsr_envelope->start_amplitude
            );
        }

        // Sustain
        if (lifetime_s > (adsr_envelope->attack_time_s + adsr_envelope->decay_time_s))
        {
            //puts("sustain");
            amplitude = adsr_envelope->sustain_amplitude;
        }
    } else {
        // Release
        //puts("release");
        amplitude = (
            ((time_s - adsr_envelope->trigger_off_time_s) / adsr_envelope->release_time_s)
            * (0.0f - adsr_envelope->sustain_amplitude)
            + adsr_envelope->sustain_amplitude
        );
    }

    if (amplitude <= 0.0001f)
    {
        amplitude = 0.0f;
    }
    
    return amplitude;
}

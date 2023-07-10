#include "adsr_envelope.h"
#include <stdlib.h>
#include <stdio.h>

/*
 *     |  |    |      Start amplitude
 *    /|\ |    |
 *   / | \|____|      Sustain amplitude
 *  /  |  |    |\
 * /   |  |    | \
 *  A   D   S    R 
 */

ADSREnvelope*
adsr_envelope_new(
    float attack_time_s,
    float decay_time_s,
    float release_time_s,
    float sustain_amplitude,
    float start_amplitude
) {
    ADSREnvelope* adsr_envelope = malloc(sizeof(ADSREnvelope));
    if (adsr_envelope == NULL)
    {
        //
    }

    adsr_envelope->attack_time_s = attack_time_s;
    adsr_envelope->decay_time_s = decay_time_s;
    adsr_envelope->release_time_s = release_time_s;

    adsr_envelope->sustain_amplitude = sustain_amplitude;
    adsr_envelope->start_amplitude = start_amplitude;

    adsr_envelope->trigger_on_time_s = 0.0f;
    adsr_envelope->trigger_off_time_s = 0.0f;

    adsr_envelope->note_enabled = false;

    return adsr_envelope;
}

static inline float
zero_below_threshold(float value)
{
    return (value <= 0.0001f) ? 0.0f : value;
}

bool
adsr_envelope_finished(const ADSREnvelope* adsr_envelope, float time_s)
{
    const float time_since_trigger_off = (
        time_s - adsr_envelope->trigger_off_time_s
    );

    return (
        !adsr_envelope->note_enabled
        && time_since_trigger_off > adsr_envelope->release_time_s
    );
}
//enum ADSRStage
//adsr_envelope_stage(const ADSREnvelope* adsr_envelope, float time_s)
//{
//    const float lifetime_s =
//        (adsr_envelope->note_enabled)
//        ? time_s - adsr_envelope->trigger_on_time_s
//        : adsr_envelope->trigger_off_time_s - adsr_envelope->trigger_on_time_s;
//    const float end_decay_time_s = (
//        adsr_envelope->attack_time_s + adsr_envelope->decay_time_s
//    );
//
//    if (adsr_envelope->note_enabled)
//    {
//        if (lifetime_s <= adsr_envelope->attack_time_s)
//        {
//            return STAGE_ATTACK;
//        }
//        else if (
//            lifetime_s > adsr_envelope->attack_time_s
//            && lifetime_s <= end_decay_time_s
//        )
//        {
//            return STAGE_DECAY;
//        }
//        else if (lifetime_s > end_decay_time_s)
//        {
//            return STAGE_SUSTAIN;
//        }
//    }
//    else
//    {
//        const float time_since_trigger_off = (
//            time_s - adsr_envelope->trigger_off_time_s
//        );
//
//        if (zero_below_threshold(lifetime_s) == 0.0f)
//        {
//            return STAGE_UNINITIATED;            
//        }
//        else if (time_since_trigger_off > adsr_envelope->release_time_s)
//        {
//            return STAGE_FINISHED;
//        }
//        else
//        {
//            return STAGE_RELEASE;
//        }
//    }
//}

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
adsr_envelope_amplitude(const ADSREnvelope* adsr_envelope, float time_s)
{
    float amplitude = 0.0f;
    const float lifetime_s = time_s - adsr_envelope->trigger_on_time_s;
    const float end_decay_time_s = (
        adsr_envelope->attack_time_s + adsr_envelope->decay_time_s
    );

 //   switch (adsr_envelope_stage(adsr_envelope, time_s))
 //   {
 //       case STAGE_UNINITIATED:
 //          // puts("uninitiated");
 //           amplitude = 0.0f;
 //           break;
 //       case STAGE_ATTACK:
 //           //puts("attack");
 //           amplitude = (
 //               (lifetime_s / adsr_envelope->attack_time_s)
 //               *
 //               adsr_envelope->start_amplitude
 //           );
 //           break;
 //       case STAGE_DECAY:
 //          // puts("decay");
 //           const float decay_time_progress = (
 //               (lifetime_s - adsr_envelope->attack_time_s)
 //               /
 //               adsr_envelope->decay_time_s
 //           );
 //           const float amplitude_decrease = (
 //               adsr_envelope->start_amplitude - adsr_envelope->sustain_amplitude
 //           );

 //           amplitude = (
 //               adsr_envelope->start_amplitude
 //               -
 //               decay_time_progress * amplitude_decrease
 //           );

 //           break;
 //       case STAGE_SUSTAIN:
 //        //   puts("sustain");
 //           amplitude = adsr_envelope->sustain_amplitude;
 //           break;
 //       case STAGE_RELEASE:
 //         //  puts("RELEASE");
 //           amplitude = (
 //               amplitude
 //               *
 //               (
 //                   1.0f
 //                   -
 //                   (time_s - adsr_envelope->trigger_off_time_s)
 //                   /
 //                   adsr_envelope->release_time_s
 //               )
 //           );
 //           break;
 //       case STAGE_FINISHED:
 //          // puts("FINISHED");
 //           break;
 //   }

    // Attack
    if (lifetime_s <= adsr_envelope->attack_time_s)
    {
        if (adsr_envelope->attack_time_s == 0.0f)
            return adsr_envelope->start_amplitude;

        amplitude = (
            (lifetime_s / adsr_envelope->attack_time_s)
            *
            adsr_envelope->start_amplitude
        );
    }
    // Decay
    else if (lifetime_s > adsr_envelope->attack_time_s && lifetime_s <= end_decay_time_s)
    {
        const float decay_time_progress = (
            (lifetime_s - adsr_envelope->attack_time_s)
            /
            adsr_envelope->decay_time_s
        );
        const float amplitude_decrease = (
            adsr_envelope->start_amplitude - adsr_envelope->sustain_amplitude
        );

        amplitude = (
            adsr_envelope->start_amplitude
            -
            decay_time_progress * amplitude_decrease
        );
    }
    // Sustain
    else if (lifetime_s > end_decay_time_s)
    {
        amplitude = adsr_envelope->sustain_amplitude;
    }

    if (adsr_envelope->note_enabled)
    {
        return zero_below_threshold(amplitude);
    }
    else
    {
        return zero_below_threshold(
            amplitude
            *
            (1.0f - (time_s - adsr_envelope->trigger_off_time_s) / adsr_envelope->release_time_s)
        );
    }
}

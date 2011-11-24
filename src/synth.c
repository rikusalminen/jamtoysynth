#include <string.h>
#include <math.h>

#include <synth.h>

int synth_init(synth_t *synth, int sample_rate)
{
    memset(synth, 0, sizeof(synth_t));
    synth->sample_rate = sample_rate;

    for(instrument_control_t *controls = synth->controls;
        controls != synth->controls + SYNTH_MAX_INSTRUMENTS;
        ++controls)
    {
        controls->modulation = MODULATION_VIBRATO;
        controls->carrier = OSCILLATOR_SINE;
        controls->carrier_amplitude = 0.6;
        controls->modulator = OSCILLATOR_SQUARE;
        controls->modulator_freq = 2.0;
        controls->modulator_amplitude = 0.6;

        controls->attack = 0.1;
        controls->decay = 1.0;
        controls->sustain = 0.7;
        controls->release = 1.5;

        controls->filter = FILTER_LOWPASS;
        controls->filter_freq = 500.0;
        controls->filter_resonance = 0.5;
        controls->filter_gain = 0.5;
    }

    return 0;
}

void synth_cleanup(synth_t *synth)
{
    (void)synth;
}

void synth_control(synth_t *synth)
{
    for(int i = 0; i < SYNTH_MAX_INSTRUMENTS; ++i)
        instrument_control(synth->instruments + i, synth->controls + i, synth->sample_rate);
}

void synth_mix(synth_t *synth, float *left, float *right)
{
    *left = 0; *right = 0;

    for(instrument_t *instrument = synth->instruments;
        instrument != synth->instruments + SYNTH_MAX_INSTRUMENTS;
        ++instrument)
    {
        float l, r;
        instrument_play(instrument, synth->sample_rate, &l, &r);
        *left += l; *right += r;
    }
}

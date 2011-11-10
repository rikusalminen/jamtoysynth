#include <string.h>
#include <math.h>

#include <synth.h>

int synth_init(synth_t *synth, int sample_rate)
{
    memset(synth, 0, sizeof(synth_t));
    synth->sample_rate = sample_rate;


    for(instrument_t *instrument = synth->instruments;
        instrument != synth->instruments + SYNTH_MAX_INSTRUMENTS;
        ++instrument)
    {
        instrument->modulation = MODULATION_FREQUENCY;
        instrument->carrier.waveform = OSCILLATOR_TRIANGLE;
        instrument->carrier.amplitude = 0.7;
        instrument->modulator.waveform = OSCILLATOR_SINE;
        instrument->modulator.freq = 2.0;
        instrument->modulator.amplitude = 0.6;

        adsr_set(&instrument->adsr, sample_rate, 0.1, 1.0, 0.7, 1.5);
        filter_set(&instrument->filter, sample_rate, FILTER_LOWPASS, 500, 0.5, 0.5);
    }

    return 0;
}

void synth_cleanup(synth_t *synth)
{
    (void)synth;
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

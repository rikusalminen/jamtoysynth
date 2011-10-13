#include <math.h>

#include <synth.h>

int synth_init(synth_t *synth, int sample_rate)
{
    synth->sample_rate = sample_rate;
    synth->phase = 0;
    return 0;
}

void synth_cleanup(synth_t *synth)
{
    (void)synth;
}

static float note_freq(int i)
{
    const float A = 440.0;
    return (A / 32.0) * pow(2.0, (i - 9) / 12.0);
}

void synth_mix(synth_t *synth, float *left, float *right)
{
    float sample = sinf(synth->phase * 2.0 * M_PI);
    float freq = note_freq(60);
    synth->phase +=  freq * 1.0 / synth->sample_rate;

    *left = sample;
    *right = sample;
}

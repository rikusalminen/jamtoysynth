#ifndef SYNTH_H
#define SYNTH_H

typedef struct synth_t
{
    int sample_rate;
    float phase;
} synth_t;

int synth_init(synth_t *synth, int sample_rate);
void synth_cleanup(synth_t *synth);

void synth_mix(synth_t *synth, float *left, float *right);

#endif

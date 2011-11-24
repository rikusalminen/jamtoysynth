#ifndef SYNTH_H
#define SYNTH_H

#include <instrument.h>

#define SYNTH_MAX_INSTRUMENTS 16

typedef struct synth_t
{
    int sample_rate;
    instrument_control_t controls[SYNTH_MAX_INSTRUMENTS];
    instrument_t instruments[SYNTH_MAX_INSTRUMENTS];
} synth_t;

int synth_init(synth_t *synth, int sample_rate);
void synth_cleanup(synth_t *synth);

void synth_control(synth_t *synth);
void synth_mix(synth_t *synth, float *left, float *right);

#endif

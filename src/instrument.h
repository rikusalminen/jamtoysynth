#ifndef INSTRUMENT_H
#define INSTRUMENT_H

typedef struct instrument_t
{
    float freq;
    float phase;
    float amplitude;
} instrument_t;

void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right);

#endif

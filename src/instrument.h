#ifndef INSTRUMENT_H
#define INSTRUMENT_H

typedef struct instrument_t
{
    float freq;
    float phase;
    float amplitude;

    float attackG, releaseG, decayG;
    float sustain, decay;
    float adsrG, adsrX;
    int decay_timer;
    float envelope;
} instrument_t;

void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right);

void instrument_adsr(instrument_t *instrument, int sample_rate, float attack, float decay, float sustain, float release);
void instrument_trigger(instrument_t *instrument);

#endif

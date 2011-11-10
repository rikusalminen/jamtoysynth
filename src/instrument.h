#ifndef INSTRUMENT_H
#define INSTRUMENT_H

typedef enum oscillator_waveform_t
{
    OSCILLATOR_ZERO = 0,
    OSCILLATOR_SINE,
    OSCILLATOR_SQUARE,
    OSCILLATOR_SAWTOOTH,
    OSCILLATOR_TRIANGLE,
    OSCILLATOR_NOISE,
} oscillator_waveform_t;

typedef struct oscillator_t
{
    oscillator_waveform_t waveform;
    float freq;
    float phase;
    float amplitude;
} oscillator_t;

float oscillator(oscillator_t *osc, int sample_rate);

typedef struct adsr_t
{
    float attackG, releaseG, decayG;
    float sustain, decay;
    float adsrG, adsrX;
    int decay_timer;
    float envelope;
} adsr_t;

void adsr_set(adsr_t *adsr, int sample_rate, float attack, float decay, float sustain, float release);
void adsr_trigger(adsr_t *adsr);
float adsr_envelope(adsr_t *adsr);

typedef struct filter_state_t
{
    float x1, x2, y1, y2;
} filter_state_t;

typedef struct filter_t
{
    float b0, b1, b2, a0, a1, a2;
} filter_t;

typedef enum filter_type_t
{
    FILTER_NONE,
    FILTER_LOWPASS,
    FILTER_HIGHPASS,
    FILTER_BANDPASS,
    FILTER_NOTCH,
    FILTER_PEAKING_EQ,
    FILTER_LOW_SHELF,
    FILTER_HIGH_SHELF
} filter_type_t;

void filter_set(filter_t *filter, int sample_rate, filter_type_t type, float f0, float Q, float dBgain);
float filter(const filter_t *filter, filter_state_t *state, float sample);

typedef struct instrument_t
{
    oscillator_t oscillator;
    adsr_t adsr;

    filter_t filter;
    filter_state_t filter0, filter1;
} instrument_t;

void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right);

#endif

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#define ECHO_BUFFER_SIZE 96000 // 2 seconds @ 48 kHz

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

float oscillator(oscillator_t *osc, int sample_rate, float freq_mod);

typedef enum modulation_t
{
    MODULATION_NONE = 0,
    MODULATION_AMPLITUDE,
    MODULATION_FREQUENCY,
    MODULATION_RING,
    MODULATION_TREMOLO,
    MODULATION_VIBRATO,
} modulation_t;

float modulate(modulation_t modulation, int sample_rate, oscillator_t *carrier, oscillator_t *modulator);

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
    FILTER_NONE = 0,
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

typedef struct echo_t
{
    float buffer[ECHO_BUFFER_SIZE];
    int cursor;
    int delay_samples;
    float feedback, level;
} echo_t;

float echo(echo_t *echo, float sample);

typedef struct instrument_control_t
{
    modulation_t modulation;
    oscillator_waveform_t carrier;
    float carrier_amplitude;
    oscillator_waveform_t modulator;
    float modulator_amplitude, modulator_freq;

    float attack, decay, sustain, release;

    filter_type_t filter;
    float filter_freq, filter_resonance, filter_gain;

    float echo_delay, echo_feedback, echo_level;
} instrument_control_t;

typedef struct instrument_t
{
    modulation_t modulation;
    oscillator_t carrier, modulator;
    adsr_t adsr;

    filter_t filter;
    filter_state_t filter0, filter1;

    echo_t echo;
} instrument_t;

void instrument_control(instrument_t *instrument, const instrument_control_t *control, int sample_rate);
void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right);

#endif

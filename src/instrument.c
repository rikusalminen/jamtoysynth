#include <stdlib.h>
#include <math.h>

#include <instrument.h>

static float zero(float phase) { (void)phase; return 0; }
static float sine(float phase) { return sinf(phase); }
static float square(float phase) { return phase <= M_PI ? 1.0 : -1.0; }
static float sawtooth(float phase) { return -1.0 + 2.0 * fmod(phase / M_PI, 1.0); }
static float noise(float phase) { (void)phase; return rand() / (float)RAND_MAX; }
static float triangle(float phase)
{
    if(phase < M_PI/2.0) return phase / (M_PI/2.0);
    if(phase <= 3.0*M_PI/2.0) return 1.0 - 2.0 * (phase-M_PI/2.0)/M_PI;
    return -1.0 + (phase - 3.0*M_PI/2.0) / (M_PI/2.0);
}

float oscillator(oscillator_t *osc, int sample_rate)
{
    typedef float (*wave_fun)(float);
    wave_fun waves[] = { zero, sine, square, sawtooth, triangle, noise };
    float sample = osc->amplitude * waves[osc->waveform](osc->phase);
    osc->phase = fmod(osc->phase + osc->freq * 2.0 * M_PI / sample_rate, 2.0 * M_PI);
    return sample;
}

void adsr_set(adsr_t *adsr, int sample_rate, float attack, float decay, float sustain, float release)
{
    adsr->attackG = expf(-1.0 / (attack * sample_rate));
    adsr->decayG = expf(-1.0 / (decay * sample_rate));
    adsr->releaseG = expf(-1.0 / (release * sample_rate));
    adsr->sustain = sustain;
    adsr->decay = decay * sample_rate;
    adsr->decay_timer = 0;

    adsr->adsrX = 0;
    adsr->adsrG = 0;
}

void adsr_trigger(adsr_t *adsr)
{
    adsr->adsrG = adsr->attackG;
    adsr->adsrX = 1.5; // TODO: play with this magic constant (velocity?)
    adsr->decay_timer = 0;
}

float adsr_envelope(adsr_t *adsr)
{
    adsr->envelope = adsr->adsrG * (adsr->envelope - adsr->adsrX) + adsr->adsrX;

    if(adsr->envelope >= 1.0)
    {
        adsr->adsrG = adsr->decayG;
        adsr->adsrX = adsr->sustain;
        adsr->decay_timer = adsr->decay;
        adsr->envelope = 1.0;
    }

    adsr->decay_timer -= 1;

    if(adsr->decay_timer == 0)
    {
        adsr->adsrG = adsr->releaseG;
        adsr->adsrX = 0.0;
    }

    return adsr->envelope;
}

void filter_set(filter_t *filter, int sample_rate, filter_type_t type, float f0, float Q, float dBgain)
{
    double cosw0 = cos(2*M_PI*f0/sample_rate);
    double sinw0 = sqrt(1.0 - cosw0 * cosw0); //sin(2*M_PI*f0/SAMPLE_RATE);
    double alpha = sinw0 / (2*Q);
    double A = pow(10.0, dBgain/40);
    double A2 = 2*sqrt(A);
    double a0, a1, a2, b0, b1, b2;

    switch (type)
    {
        case FILTER_LOWPASS:
            b0 = (1 - cosw0)/2;
            b1 = 1 - cosw0;
            b2 = (1 - cosw0)/2;
            a0 = 1 + alpha;
            a1 = -2.0 * cosw0;
            a2 = 1 - alpha;
            break;
        case FILTER_HIGHPASS:
            b0 = (1 + cosw0)/2;
            b1 = -(1 + cosw0);
            b2 = (1 + cosw0)/2;
            a0 = 1 + alpha;
            a1 = -2 * cosw0;
            a2 = 1 - alpha;
            break;
        case FILTER_BANDPASS:
            b0 = alpha;
            b1 = 0;
            b2 = -alpha;
            a0 = 1 + alpha;
            a1 = -2 * cosw0;
            a2 = 1 - alpha;
            break;
        case FILTER_NOTCH:
            b0 = 1;
            b1 = -2*cosw0;
            b2 = 1;
            a0 = 1 + alpha;
            a1 = -2*cosw0;
            a2 = 1-alpha;
            break;
        case FILTER_PEAKING_EQ:
            b0 = 1 + alpha*A;
            b1 = -2*cosw0;
            b2 = 1 - alpha*A;
            a0 = 1 + alpha/A;
            a1 = -2*cosw0;
            a2 = 1 - alpha/A;
            break;
        case FILTER_LOW_SHELF:
            b0 = A*((A+1) - (A-1)*cosw0 + A2 * alpha);
            b1 = 2*A*((A-1) - (A+1) * cosw0);
            b2 = A*((A+1) - (A-1) * cosw0 - A2 * alpha);
            a0 = (A+1) + (A-1) * cosw0 + A2 * alpha;
            a1 = -2*((A-1) + (A+1) * cosw0);
            a2 = (A+1) + (A-1) * cosw0 - A2 * alpha;
            break;
        case FILTER_HIGH_SHELF:
            b0 = A*((A+1) + (A-1) * cosw0 + A2 * alpha);
            b1 = -2*A*((A-1) + (A+1) * cosw0);
            b2  = A*((A+1) + (A-1) * cosw0 - A2 * alpha);
            a0 = (A+1) - (A-1) * cosw0 + A2 * alpha;
            a1 = 2*((A-1) - (A+1) * cosw0);
            a2 = (A+1) - (A-1) * cosw0 - A2 * alpha;
            break;
        case FILTER_NONE:
        default:
            b0 = a0 = 1.0;
            b1 = b2 = 0.0;
            a1 = a2 = 0.0;
            break;
    }

    filter->a0 = a0 * 64.0;
    filter->a1 = -a1 * 64.0;
    filter->a2 = -a2 * 64.0;
    filter->b0 = b0 * 64.0;
    filter->b1 = b1 * 64.0;
    filter->b2 = b2 * 64.0;
}

float filter(const filter_t *filter, filter_state_t *state, float sample)
{
    float out = (
        filter->b0 * sample +
        filter->b1 * state->x1 +
        filter->b2 * state->x2 +
        filter->a1 * state->y1 +
        filter->a2 * state->y2) /
        filter->a0;

    state->x2 = state->x1;
    state->x1 = sample;
    state->y2 = state->y1;
    state->y1 = out;

    return out;
}

void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right)
{
    float sample =
        filter(&instrument->filter, &instrument->filter1,
            filter(&instrument->filter, &instrument->filter0,
                adsr_envelope(&instrument->adsr) *
                oscillator(&instrument->oscillator, sample_rate)));

    *left = sample;
    *right = sample;
}

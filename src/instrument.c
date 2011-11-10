#include <math.h>

#include <instrument.h>

void instrument_adsr(instrument_t *instrument, int sample_rate, float attack, float decay, float sustain, float release)
{
    instrument->attackG = expf(-1.0 / (attack * sample_rate));
    instrument->decayG = expf(-1.0 / (decay * sample_rate));
    instrument->releaseG = expf(-1.0 / (release * sample_rate));
    instrument->sustain = sustain;
    instrument->decay = decay * sample_rate;
    instrument->decay_timer = 0;

    instrument->adsrX = 0;
    instrument->adsrG = 0;
}

void instrument_trigger(instrument_t *instrument)
{
    instrument->adsrG = instrument->attackG;
    instrument->adsrX = 1.5; // TODO: play with this magic constant
    instrument->decay_timer = 0;
}

static void instrument_envelope(instrument_t *instrument)
{
    instrument->envelope = instrument->adsrG * (instrument->envelope - instrument->adsrX) + instrument->adsrX;

    if(instrument->envelope >= 1.0)
    {
        instrument->adsrG = instrument->decayG;
        instrument->adsrX = instrument->sustain;
        instrument->decay_timer = instrument->decay;
        instrument->envelope = 1.0;
    }

    instrument->decay_timer -= 1;

    if(instrument->decay_timer == 0)
    {
        instrument->adsrG = instrument->releaseG;
        instrument->adsrX = 0.0;
    }
}

void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right)
{
    instrument_envelope(instrument);

    float sample = instrument->envelope * instrument->amplitude * sinf(instrument->phase);
    instrument->phase = fmod(instrument->phase + instrument->freq * 2.0 * M_PI / sample_rate, 2.0 * M_PI);

    *left = sample;
    *right = sample;
}

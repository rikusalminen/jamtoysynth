#include <math.h>

#include <instrument.h>

void instrument_play(instrument_t *instrument, int sample_rate, float *left, float *right)
{
    float sample = instrument->amplitude * sinf(instrument->phase * 2.0 * M_PI);
    instrument->phase = fmod(instrument->phase + instrument->freq * 1.0 / sample_rate, 2.0 * M_PI * sample_rate);

    *left = sample;
    *right = sample;
}

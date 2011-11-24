#include <stdbool.h>
#include <assert.h>
#include <SDL/SDL.h>

#include <synth.h>
#include <audio_output.h>

static int format_bits(uint16_t format)
{
    switch(format)
    {
        case AUDIO_U8:
        case AUDIO_S8:
            return 8;
        case AUDIO_U16LSB:
        case AUDIO_S16LSB:
        case AUDIO_U16MSB:
        case AUDIO_S16MSB:
            return 16;
        default:
            assert(false && "Unknown audio format");
    }
}

static bool format_signed(uint16_t format)
{
    switch(format)
    {
        case AUDIO_S16LSB:
        case AUDIO_S8:
            return true;
        case AUDIO_U8:
        case AUDIO_U16LSB:
            return false;
        default:
            assert(false && "Unknown audio format");
    }
}

static bool format_swap_endian(uint16_t format)
{
    switch(format)
    {
        case AUDIO_U8:
        case AUDIO_S8:
            return false;
        case AUDIO_U16LSB:
        case AUDIO_S16LSB:
            return (AUDIO_U16LSB != AUDIO_U16SYS);
        case AUDIO_U16MSB:
        case AUDIO_S16MSB:
            return (AUDIO_U16MSB != AUDIO_U16SYS);
        default:
            assert(false && "Unknown audio format");
    }
}

void audio_callback(void *userdata, uint8_t *stream, int len)
{
    audio_callback_userdata_t *params = (audio_callback_userdata_t*)userdata;

    int bits = format_bits(params->audiospec.format);
    bool sign = format_signed(params->audiospec.format);
    bool swap_endian = format_swap_endian(params->audiospec.format);
    assert(!swap_endian);

    float factor = ((uint32_t)1 << (sign ? (bits-1) : bits)) - 1;
    float bias = sign ? 0.0 : 1.0;

    int num_samples = len / (bits/8) / params->audiospec.channels;

    synth_control(params->synth);

    for(int i = 0; i < num_samples; ++i)
    {
        float out_samples[2];
        synth_mix(params->synth, out_samples+0, out_samples+1);

        if(params->audiospec.channels == 1) // downmix to mono
            out_samples[0] = (out_samples[0] + out_samples[1]) / 2.0;

        for(int j = 0; j < params->audiospec.channels; ++j)
        {
            float sample = factor * (out_samples[j] + bias);
            int32_t isample = sample;
            /*printf("%2.3f\t%10d\n", sample, isample);*/

            for(int byte = 0; byte < (bits/8); ++byte)
                *(stream + params->audiospec.channels * (bits/8) * i + j * (bits/8) + byte) =
                    *((uint8_t*)&isample + byte);
        }
    }
}

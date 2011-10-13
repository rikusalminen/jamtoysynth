#ifndef AUDIO_CALLBACK_H
#define AUDIO_CALLBACK_H

typedef struct audio_callback_userdata_t
{
    SDL_AudioSpec audiospec;
    synth_t *synth;
} audio_callback_userdata_t;

void audio_callback(void *userdata, uint8_t *stream, int len);

#endif

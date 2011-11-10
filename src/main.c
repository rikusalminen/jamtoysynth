#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <synth.h>
#include <audio_output.h>

static float note_freq(int i)
{
    const float A = 440.0;
    return (A / 32.0) * pow(2.0, (i - 9) / 12.0);
}

static int main_loop(synth_t *synth)
{
    const SDLKey keyboard[] = {
    //  c       c#      d       d#      e       f       f#      g       g#      a       a#      b       c       d       e
        SDLK_z, SDLK_s, SDLK_x, SDLK_d, SDLK_c, SDLK_v, SDLK_g, SDLK_b, SDLK_h, SDLK_n, SDLK_j, SDLK_m, SDLK_COMMA, SDLK_l, SDLK_PERIOD,
        SDLK_q, SDLK_2, SDLK_w, SDLK_3, SDLK_e, SDLK_r, SDLK_5, SDLK_t, SDLK_6, SDLK_y, SDLK_7, SDLK_u, SDLK_i, SDLK_9, SDLK_o, SDLK_0, SDLK_p
    };
    int notes[] = {
    //  c       c#      d       d#      e       f       f#      g       g#      a       a#      b       c       d       e
        0,      1,      2,      3,      4,      5,      6,      7,      8,      9,      10,     11,     12,     13,     14,
        12,    13,     14,     15,     16,     17,     18,     19,     20,     21,      22,     23,     24,     25,     26,     27,     28,
    };

    int num_keys = sizeof(keyboard)/sizeof(*keyboard);
    int base_octave = 4;

    bool quit = false;
    while(!quit)
    {
        const float background[] = { 0.2, 0.4, 0.7, 1.0 };
        glClearBufferfv(GL_COLOR, 0, background);

        SDL_GL_SwapBuffers();

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
            else if(event.type == SDL_KEYDOWN && event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F12)
                base_octave = event.key.keysym.sym - SDLK_F1;
            else if(event.type == SDL_KEYDOWN)
            {
                for(int i = 0; i < num_keys; ++i)
                {
                    if(keyboard[i] != event.key.keysym.sym) continue;

                    SDL_LockAudio();
                    synth->instruments->carrier.freq = note_freq(base_octave * 12 + notes[i]);
                    adsr_trigger(&synth->instruments[0].adsr);
                    SDL_UnlockAudio();
                }
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        return -1;
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

    SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);
    if(!screen)
        return -1;

    if(glewInit() != GLEW_OK)
        return -1;

    audio_callback_userdata_t audio_callback_userdata;

    SDL_AudioSpec desired_audio_spec, obtained_audio_spec;
    desired_audio_spec.freq = 44100;
    desired_audio_spec.format = AUDIO_U16SYS;
    desired_audio_spec.channels = 1;
    desired_audio_spec.samples = 1024;
    desired_audio_spec.callback = audio_callback;
    desired_audio_spec.userdata = &audio_callback_userdata;

    if(SDL_OpenAudio(&desired_audio_spec, &obtained_audio_spec) != 0)
        return -1;
    atexit(SDL_CloseAudio);

    synth_t synth;
    synth_init(&synth, obtained_audio_spec.freq);

    audio_callback_userdata.synth = &synth;
    audio_callback_userdata.audiospec = obtained_audio_spec;

    SDL_PauseAudio(0);

    int err = main_loop(&synth);

    SDL_PauseAudio(1);

    synth_cleanup(&synth);

    SDL_CloseAudio();
    SDL_Quit();

    return err;
}


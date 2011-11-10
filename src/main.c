#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <synth.h>
#include <audio_output.h>

static int main_loop(synth_t *synth)
{
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
            else if(event.type == SDL_KEYDOWN)
            {
                SDL_LockAudio();
                for(int i = 0; i < 3; ++i)
                    adsr_trigger(&synth->instruments[i].adsr);
                SDL_UnlockAudio();
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


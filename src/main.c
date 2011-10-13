#include <stdbool.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

typedef struct audio_callback_userdata_t
{
    SDL_AudioSpec audiospec;
} audio_callback_userdata_t;

static void audio_callback(void *userdata, uint8_t *stream, int len)
{
    for(int i = 0; i < len; ++i)
        stream[i] = rand() % 0xff;
}

static int main_loop()
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
    desired_audio_spec.channels = 2;
    desired_audio_spec.samples = 1024;
    desired_audio_spec.callback = audio_callback;
    desired_audio_spec.userdata = &audio_callback_userdata;

    if(SDL_OpenAudio(&desired_audio_spec, &obtained_audio_spec) != 0)
        return -1;
    atexit(SDL_CloseAudio);
    audio_callback_userdata.audiospec = obtained_audio_spec;

    SDL_PauseAudio(0);

    int err = main_loop();

    SDL_PauseAudio(1);

    SDL_CloseAudio();
    SDL_Quit();

    return err;
}


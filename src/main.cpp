#include <chrono>
#include <iostream>
#include <thread>
#include <cmath>
#include <SDL2/SDL.h>

void callback(void* userdata, Uint8* stream, int len)
{
    auto samples = reinterpret_cast<float*>(stream);
    auto num_samples = len / sizeof(float);

    // Info
    //desired_specs.freq = 48000;
    //desired_specs.format = AUDIO_F32;
    //desired_specs.channels = 2;
    //desired_specs.samples = 1024;

    static float current_phase = 0.0f;
    float freq = 440;
    float delta = (freq / 48000.0f) * 2.0f * (float)M_PI;

    for (size_t i = 0; i < num_samples; ++i)
    {
        auto current_sample = sin(current_phase);
        current_phase += delta;
        samples[i] = current_sample;
    }
}

int main()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL_Init(): " << SDL_GetError() << std::endl;
        SDL_ClearError();
        return 1;
    }

    SDL_AudioSpec desired_specs = {};
    SDL_AudioSpec given_specs = {};
    desired_specs.freq = 48000;
    desired_specs.format = AUDIO_F32;
    desired_specs.channels = 1;
    desired_specs.samples = 1024;
    desired_specs.callback = callback;

    const int audio_dev_id = SDL_OpenAudioDevice(nullptr, 0, &desired_specs,
                                                 &given_specs, SDL_AUDIO_ALLOW_CHANNELS_CHANGE);

    if(audio_dev_id == 0)
    {
        std::cerr << "Error opening audio device: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Play
    SDL_PauseAudioDevice(audio_dev_id, 0);

    // Loop
    SDL_Event ev;
    while(SDL_WaitEvent(&ev))
    {
        if(ev.type == SDL_QUIT)
        {
            break;
        }
    }
}
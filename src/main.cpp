#include <chrono>
#include <iostream>
#include <vector>

#include <cmath>

#include <SDL.h>

struct block;
struct audio_graph;

struct block
{
    virtual float step(audio_graph* spec, float input) = 0;
};

struct audio_graph
{
    std::vector<block*> blocks;
    SDL_AudioSpec* spec;
    bool paused;
    bool dirty;
};

struct sine_wave_block : public block
{
    float step(audio_graph* graph, float input) override
    {
        delta = (sine_freq / graph->spec->freq) * 2.0f * (float)M_PI;
        auto current_sample = amplitude * sin(input + current_phase);
        current_phase = fmod(current_phase + delta, (2.0f * (float)M_PI));
        return current_sample;
    }

    float amplitude = 1.0f;
    float sine_freq = 440.0f;
    float current_phase = 0.0f;
    float delta = 0.0f;
};

struct soft_clip_block : public block
{
    float step(audio_graph* spec, float input) override
    {
        return tanh(input);
    }
};

void callback(void* user_data, Uint8* stream, int len)
{
    auto graph = reinterpret_cast<audio_graph*>(user_data);
    auto samples = reinterpret_cast<float*>(stream);
    auto num_samples = len / sizeof(float);

    for (size_t i = 0; i < num_samples; i += graph->spec->channels)
    {
        auto current_sample = 0.0f;

        for (auto& block : graph->blocks)
        {
            current_sample = block->step(graph, current_sample);
        }

        for (int channel = 0; channel < graph->spec->channels; ++channel)
        {
            samples[i + channel] = current_sample;
        }
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

    audio_graph graph;

    SDL_AudioSpec desired_specs = {};
    SDL_AudioSpec given_specs = {};

    desired_specs.freq = 48000;
    desired_specs.format = AUDIO_F32;
    desired_specs.channels = 2;
    desired_specs.samples = 1024;
    desired_specs.callback = callback;
    desired_specs.userdata = &graph;

    const int audio_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired_specs,
                                                 &given_specs, 0);

    if (audio_device_id == 0)
    {
        std::cerr << "Error opening audio device: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set up initial graph
    graph.spec = &given_specs;

    auto a = sine_wave_block();
    auto b = soft_clip_block();

    graph.blocks.push_back(&a);
    graph.blocks.push_back(&b);

    // Play
    SDL_PauseAudioDevice(audio_device_id, graph.paused);

    // Loop
    SDL_Event ev;
    while(SDL_WaitEvent(&ev))
    {
        if(ev.type == SDL_QUIT)
        {
            break;
        }

        // UI interaction that can set state to dirty

        // Logic
        if (graph.dirty)
        {
            // Update
        }

        // Render
    }
}
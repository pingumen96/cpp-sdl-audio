#pragma once

// AudioSample.h
/*
This class encapsulates an audio sample loaded from a WAV file with SDL2.
It manages its data using smart pointers with a custom deleter to ensure proper
cleanup.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include <memory>
#include <string>

class AudioSample {
   public:
    explicit AudioSample(const std::string& filePath);

    // move constructor and move assignment operator
    AudioSample(AudioSample&& other) noexcept;
    AudioSample& operator=(AudioSample&& other) noexcept;

    // no copy constructor or copy assignment operator
    AudioSample(const AudioSample&) = delete;
    AudioSample& operator=(const AudioSample&) = delete;

    // read-only access to the audio data
    const Uint8* data() const noexcept;
    Uint32 length() const noexcept;
    const SDL_AudioSpec& spec() const noexcept;

    // check validity of the sample
    bool isValid() const noexcept;

    ~AudioSample();

   private:
    static void freeWav(Uint8* ptr);

    std::shared_ptr<Uint8> buffer_;
    Uint32 length_ = 0;
    SDL_AudioSpec spec_;
};

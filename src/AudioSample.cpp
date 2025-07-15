#include "AudioSample.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include <memory>
#include <stdexcept>
#include <string>

AudioSample::AudioSample(const std::string& filePath) {
    Uint8* rawBuffer = nullptr;
    if (SDL_LoadWAV(filePath.c_str(), &spec_, &rawBuffer, &length_) ==
        nullptr) {
        throw std::runtime_error("Failed to load WAV file: " + filePath);
    }

    // the destructor of std::shared_ptr will call freeWav
    buffer_ = std::shared_ptr<Uint8>(rawBuffer, freeWav);
}

AudioSample::AudioSample(AudioSample&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      length_(other.length_),
      spec_(other.spec_) {
    other.length_ = 0;
}

AudioSample& AudioSample::operator=(AudioSample&& other) noexcept {
    if (this != &other) {
        buffer_ = std::move(other.buffer_);
        length_ = other.length_;
        spec_ = other.spec_;
        other.length_ = 0;
    }
    return *this;
}

const Uint8* AudioSample::data() const noexcept { return buffer_.get(); }

Uint32 AudioSample::length() const noexcept { return length_; }

const SDL_AudioSpec& AudioSample::spec() const noexcept { return spec_; }

bool AudioSample::isValid() const noexcept {
    return buffer_ != nullptr && length_ > 0;
}

AudioSample::~AudioSample() {
    // The shared_ptr will automatically call freeWav when the last reference is
    // released
}

void AudioSample::freeWav(Uint8* ptr) { SDL_FreeWAV(ptr); }

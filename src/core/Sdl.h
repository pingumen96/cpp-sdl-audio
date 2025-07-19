#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdexcept>
#include <string>

namespace core {

    class Sdl {
    public:
        // Constructor: initialize SDL and (optionally) subsystems
        Sdl(Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO, int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG, bool initTtf = true) {
            if (SDL_Init(sdlFlags) != 0)
                throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
            if (imgFlags) {
                if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
                    throw std::runtime_error(std::string("IMG_Init failed: ") + IMG_GetError());
            }
            if (initTtf) {
                if (TTF_Init() != 0)
                    throw std::runtime_error(std::string("TTF_Init failed: ") + TTF_GetError());
            }
            ttfInitialized = initTtf;
            imgInitialized = imgFlags;
        }

        ~Sdl() {
            // Destroy subsystems in reverse order of initialization
            if (ttfInitialized) TTF_Quit();
            if (imgInitialized) IMG_Quit();
            SDL_Quit();
        }

        // Non copiabile, non movibile (nessun senso copiare il singleton)
        Sdl(const Sdl&) = delete;
        Sdl& operator=(const Sdl&) = delete;
        Sdl(Sdl&&) = delete;
        Sdl& operator=(Sdl&&) = delete;

    private:
        bool ttfInitialized = false;
        int imgInitialized = 0;
    };

} // namespace core

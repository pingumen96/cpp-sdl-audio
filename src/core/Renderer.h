#pragma once

// src/core/Renderer.h – Renderer wrapper for SDL2
#include <SDL2/SDL.h>

#include <memory>
#include <stdexcept>

namespace core {
    class Renderer {
        std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;

    public:
        Renderer(SDL_Window* window, int index = -1, Uint32 flags = SDL_RENDERER_ACCELERATED)
            : renderer(SDL_CreateRenderer(window, index, flags), SDL_DestroyRenderer) {
            if (!renderer) {
                throw std::runtime_error(SDL_GetError());
            }
        }


        SDL_Renderer* get() const { return renderer.get(); }

        void clear();

        void present();

        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

        // Prevent copy
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        // Allow move
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;
    };
}
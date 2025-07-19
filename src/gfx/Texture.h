#pragma once

// src/core/Texture.h – Texture wrapper for SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <memory>
#include <stdexcept>


namespace gfx {
    class Texture {
        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;

    public:
        Texture(SDL_Renderer* renderer, const char* file)
            : texture(IMG_LoadTexture(renderer, file), SDL_DestroyTexture) {
            if (!texture) {
                throw std::runtime_error(IMG_GetError());
            }
        }

        SDL_Texture* get() const { return texture.get(); }

        // Prevent copy
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Allow move
        Texture(Texture&&) = default;
        Texture& operator=(Texture&&) = default;
    };
}
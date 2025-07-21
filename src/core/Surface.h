#pragma once

#include <SDL2/SDL.h>
#include <memory>

class Surface {
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface;
public:
    Surface(SDL_Surface* surf) : surface(surf, SDL_FreeSurface) {}
    SDL_Surface* get() const { return surface.get(); }
};
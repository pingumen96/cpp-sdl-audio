#include "Renderer.h"

core::Renderer::Renderer(SDL_Window* window, int index, Uint32 flags)
    : renderer(SDL_CreateRenderer(window, index, flags), SDL_DestroyRenderer) {
    if (!renderer) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDL_Renderer* core::Renderer::get() {
    return renderer.get();
}

void core::Renderer::clear() { SDL_RenderClear(renderer.get()); }
void core::Renderer::present() { SDL_RenderPresent(renderer.get()); }

void core::Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer.get(), r, g, b, a);
}
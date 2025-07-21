#include "Renderer.h"

void core::Renderer::clear() { SDL_RenderClear(renderer.get()); }
void core::Renderer::present() { SDL_RenderPresent(renderer.get()); }

void core::Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer.get(), r, g, b, a);
}
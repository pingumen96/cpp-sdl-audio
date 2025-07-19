#include "PausedState.h"
#include "PlayingState.h"
#include "MenuState.h"

void game::PausedState::handleEvent(Game& game, const SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_ESCAPE) {
            game.setState<game::PlayingState>(); // resume
        } else if (ev.key.keysym.sym == SDLK_m) {
            game.setState<game::MenuState>(); // torna al menu
        }
    }
}

void game::PausedState::render(Game& game) {
    // Render overlay (qui simulato)
    std::cout << "[Paused] Overlay trasparente. Premi ESC per riprendere.\n";
}
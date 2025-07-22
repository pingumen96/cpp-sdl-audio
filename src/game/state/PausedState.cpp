#include "PausedState.h"
#include "PlayingState.h"
#include "MenuState.h"

#include "../Game.h"
#include "../../core/Renderer.h"

#include <SDL2/SDL.h>
#include <iostream>

void game::PausedState::handleEvent(Game& game, const SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_ESCAPE) {
            game.setState<game::PlayingState>();
        } else if (ev.key.keysym.sym == SDLK_m) {
            game.setState<game::MenuState>(); // torna al menu
        }
    }
}

void game::PausedState::render(Game& game) {
    // Render overlay (qui simulato)
    // make blue screen
    core::Renderer& renderer = game.getRenderer();
    renderer.setDrawColor(0.0f, 0.0f, 1.0f, 0.5f); // semi-transparent blue
    renderer.clear();
    renderer.present();
    std::cout << "[PausedState] Rendering paused overlay\n";
}
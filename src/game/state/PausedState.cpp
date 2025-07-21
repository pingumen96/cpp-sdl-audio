#include "PausedState.h"
#include "PlayingState.h"
#include "MenuState.h"

#include "../Game.h"

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
    renderer.clear();
    renderer.setDrawColor(0, 0, 255, 128); // semi-transparent blue
    SDL_Rect rect = { 0, 0, 800, 600 }; // Assuming window size is 800x600
    SDL_RenderFillRect(renderer.get(), &rect);
    renderer.present();
    std::cout << "[PausedState] Rendering paused overlay\n";
}
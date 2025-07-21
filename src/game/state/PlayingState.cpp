#include "PlayingState.h"
#include "../Game.h"
#include "../../core/Renderer.h"
#include "../../gfx/Texture.h"
#include "../state/PausedState.h"


game::PlayingState::PlayingState() {} // Example texture for background

game::PlayingState::~PlayingState() = default;

void game::PlayingState::handleEvent(game::Game& game, const SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_ESCAPE) {
            game.setState<PausedState>();
            return;
        }

        game.getInputHandler().handleInput(ev.key.keysym.sym, game.getAvatar());
    }
}

void game::PlayingState::update(Game& game, float dt) {
    // update logica avatar, fisica ecc.
}

void game::PlayingState::render(Game& game) {
    // render normale

    // make red screen
    core::Renderer& renderer = game.getRenderer();
    renderer.clear();
    renderer.setDrawColor(255, 0, 0, 255); // red background
    SDL_Rect rect = { 0, 0, 800, 600 }; // Assuming window size is 800x600
    SDL_RenderFillRect(renderer.get(), &rect);
    renderer.present();

    // end image load example
}
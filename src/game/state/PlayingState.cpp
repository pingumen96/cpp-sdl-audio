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
    renderer.setDrawColor(1.0f, 0.0f, 0.0f, 1.0f); // red background
    renderer.clear();
    renderer.present();

    // end image load example
}
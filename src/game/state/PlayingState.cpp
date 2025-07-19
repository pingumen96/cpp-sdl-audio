#include "PlayingState.h"
#include "../Game.h"

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
}
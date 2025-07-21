#include "PlayingState.h"
#include "../Game.h"
#include "../../core/Renderer.h"
#include "../../gfx/Texture.h"
#include "../state/PausedState.h"


game::PlayingState::PlayingState(core::Renderer& renderer) : backgroundTexture(renderer.get(), "data/preview.png") {} // Example texture for background

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

    // image load example
    core::Renderer& renderer = game.getRenderer();
    renderer.clear();
    SDL_RenderCopy(renderer.get(), backgroundTexture.get(), nullptr, nullptr);
    renderer.present();

    // end image load example
}
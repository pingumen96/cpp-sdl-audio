// PausedState.h
#pragma once
#include "GameState.h"

namespace game {
    class Game; // forward declaration
    class PausedState : public GameState {
    public:
        const char* name() const override { return "Paused"; }
        void handleEvent(Game& game, const SDL_Event& ev) override;
        void render(Game& game) override;
    };
}
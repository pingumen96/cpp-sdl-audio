// PlayingState.h
#pragma once
#include "GameState.h"

namespace game {
    class Game; // forward declaration
    class PlayingState : public GameState {
    public:
        const char* name() const override { return "Playing"; }
        void handleEvent(Game& game, const SDL_Event& ev) override;
        void update(Game& game, float dt) override;
        void render(Game& game) override;
    };
}
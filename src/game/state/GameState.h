// GameState.h
#pragma once
#include <SDL2/SDL.h>

#include "../Game.h"

namespace game {
    class Game;
    class GameState {
    public:
        virtual ~GameState() = default;
        virtual const char* name() const = 0;

        virtual void handleEvent(Game& game, const SDL_Event& ev) {}
        virtual void update(Game& game, float dt) {}
        virtual void render(Game& game) {}
    };
}

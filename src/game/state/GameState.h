// GameState.h
#pragma once

#include <SDL2/SDL.h>

namespace game {
    class Game; // forward declaration
    class GameState {
    public:
        virtual ~GameState() = default;
        virtual const char* name() const = 0;

        // cppcheck-suppress unusedParameter
        virtual void handleEvent(Game& game, const SDL_Event& ev) {}
        virtual void update(Game& game, float dt) {}
        virtual void render(Game& game) {}

        virtual void render(Game& game, float interpolation) {
            // Default: ignore interpolation and use normal render
            render(game);
        }
    };
}

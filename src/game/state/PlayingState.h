// PlayingState.h
#pragma once

#include "GameState.h"
#include "../../gfx/Texture.h"

namespace core {        // OK
    class Window;
    class Renderer;
}

namespace game {
    // forward declarations
    class Game;
    class PlayingState : public GameState {
    public:
        PlayingState(core::Renderer& renderer);
        virtual ~PlayingState();


        const char* name() const override { return "Playing"; }
        void handleEvent(Game& game, const SDL_Event& ev) override;
        void update(Game& game, float dt) override;
        void render(Game& game) override;

    private:
        gfx::Texture backgroundTexture; // Example texture for background
    };
}
#pragma once

#include <memory>
#include <iostream>
#include "input/InputHandler.h"
#include "Avatar.h"
#include "../core/Renderer.h"

// Qui includi tutte le state complete! (Serve la definizione, non solo fwd)
#include "state/GameState.h"
#include "state/MenuState.h"
#include "state/PlayingState.h"
#include "state/PausedState.h"

namespace core {
    class Window;
}

namespace game {

    class Game {
    public:
        Game(core::Window& window);
        ~Game();

        void init();

        template<typename State, typename... Args>
        void setState(Args&&... args) {
            state = std::make_unique<State>(std::forward<Args>(args)...);
            std::cout << "[Game] State -> " << state->name() << "\n";
        }

        void mainLoop();

        input::InputHandler& getInputHandler() {
            return *inputHandler;
        }
        Avatar& getAvatar() {
            return avatar;
        }

        core::Renderer& getRenderer();

    private:
        std::unique_ptr<GameState> state; // Stato di gioco attivo
        std::unique_ptr<input::InputHandler> inputHandler;
        Avatar avatar; // Il tuo avatar di gioco
        core::Renderer renderer; // Renderer SDL2
    };

} // namespace game

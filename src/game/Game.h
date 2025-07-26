#pragma once

#include <memory>
#include <iostream>
#include <SDL2/SDL.h>
#include "input/InputHandler.h"
#include "Avatar.h"
#include "../core/Renderer.h"
#include "../scene/SceneSystem.h"

// Qui includi tutte le state complete! (Serve la definizione, non solo fwd)
#include "state/GameState.h"
#include "state/MenuState.h"
#include "state/PlayingState.h"
#include "state/PausedState.h"

namespace core {
    class Window;
}

namespace game {
    // Forward declarations for new scene system
    class MenuScene;
    class GameScene;

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

        // Scene system integration
        scene::SceneManager* getSceneManager() const { return sceneManager.get(); }

    private:
        std::unique_ptr<GameState> state; // Stato di gioco attivo (DEPRECATED - use scenes)
        std::unique_ptr<input::InputHandler> inputHandler;
        Avatar avatar; // Il tuo avatar di gioco
        core::Renderer renderer; // Renderer SDL2

        // Scene system (PRIMARY)
        std::unique_ptr<scene::SceneManager> sceneManager;

        void initializeSceneSystem();
        void handleSceneInput(const SDL_Event& event);
    };

} // namespace game
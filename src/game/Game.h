#pragma once

#include <memory>
#include <iostream>
#include <SDL2/SDL.h>
#include "input/SceneInputSystem.h"
#include "Avatar.h"
#include "../core/Renderer.h"
#include "../scene/SceneSystem.h"

// Forward declarations for scenes (no longer need old states)
namespace game {
    class MenuScene;
    class GameScene;
}

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
        void mainLoop();

        // Unified input system
        input::SceneInputSystem& getInputSystem() {
            return *inputSystem;
        }

        Avatar& getAvatar() {
            return avatar;
        }

        core::Renderer& getRenderer();

        // Scene system integration
        scene::SceneManager* getSceneManager() const { return sceneManager.get(); }

    private:
        // Unified input system (replaces old InputHandler)
        std::unique_ptr<input::SceneInputSystem> inputSystem;
        Avatar avatar; // The game avatar
        core::Renderer renderer; // SDL2/OpenGL Renderer

        // Scene system (PRIMARY - no more dual state/scene management)
        std::unique_ptr<scene::SceneManager> sceneManager;

        void initializeSceneSystem();
        void handleInput(const SDL_Event& event);
    };
} // namespace game
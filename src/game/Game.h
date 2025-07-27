#pragma once

#include <memory>
#include <iostream>
#include <SDL2/SDL.h>
#include "Avatar.h"
#include "../core/Renderer.h"
#include "../scene/SceneSystem.h"

// Forward declarations for scenes (no longer need old states)
namespace game {
    class MenuScene;
    class GameScene;
    class PongScene;
}

namespace core {
    class Window;
}

namespace game {
    // Forward declarations for new scene system
    class MenuScene;
    class GameScene;
    class PongScene;

    class Game {
    public:
        Game(core::Window& window);
        ~Game();

        void init();
        void mainLoop();

        Avatar& getAvatar() {
            return avatar;
        }

        core::Renderer& getRenderer();

        // Scene system integration
        scene::SceneManager* getSceneManager() const { return sceneManager.get(); }

    private:
        Avatar avatar; // The game avatar
        core::Renderer renderer; // SDL2/OpenGL Renderer

        // Scene system (PRIMARY - unified scene management)
        std::unique_ptr<scene::SceneManager> sceneManager;

        void initializeSceneSystem();
        void handleInput(const SDL_Event& event);
    };
} // namespace game
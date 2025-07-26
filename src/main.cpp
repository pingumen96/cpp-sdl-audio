#define SDL_MAIN_HANDLED  // disabilita il wrapping di SDL_main
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include <chrono>
#include <iostream>
#include <thread>  // per std::this_thread::sleep_for

#include "core/Renderer.h"
#include "core/Window.h"
#include "core/Surface.h"
#include "core/Sdl.h"
#include "game/Game.h"
#include "game/GameScene.h"
#include "scene/SceneSystem.h"

int main() {
    try {
        core::Sdl sdl; // Inizializza SDL2, SDL_image, SDL_ttf

        core::Window window("Hello OpenGL with Scene System", 100, 100, 800, 600);
        game::Game game(window);

        game.init();

        // Initialize and demonstrate scene system integration
        if (game.getSceneManager()) {
            std::cout << "\n=== Scene System Integration Demo ===" << std::endl;

            // Create and push a game scene
            auto gameScene = std::make_unique<game::GameScene>();
            game.getSceneManager()->pushScene(std::move(gameScene));

            std::cout << "Game scene pushed to scene manager" << std::endl;

            // Run a few scene system update/render cycles
            std::cout << "\nRunning scene system cycles..." << std::endl;
            const float deltaTime = 1.0f / 60.0f;

            for (int i = 0; i < 5; ++i) {
                std::cout << "\n--- Scene Cycle " << (i + 1) << " ---" << std::endl;
                game.getSceneManager()->update(deltaTime);
                bool renderSuccess = game.getSceneManager()->render();

                if (!renderSuccess) {
                    std::cerr << "Scene render failed!" << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            std::cout << "\nScene system demo completed!" << std::endl;
        } else {
            std::cerr << "Scene manager not available, falling back to regular game loop" << std::endl;
        }

        // Run the normal game loop
        game.mainLoop();

    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

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
#include "scenes/GameScene.h"
#include "scene/SceneSystem.h"

int main() {
    try {
        core::Sdl sdl; // Inizializza SDL2, SDL_image, SDL_ttf

        core::Window window("Hello OpenGL with Scene System", 100, 100, 800, 600);
        game::Game game(window);

        game.init();

        // Skip the demo and go directly to main game loop
        std::cout << "\n=== Starting Game ===" << std::endl;

        // Run the normal game loop
        game.mainLoop();

    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

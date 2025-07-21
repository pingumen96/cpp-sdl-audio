#define SDL_MAIN_HANDLED  // disabilita il wrapping di SDL_main
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <windows.h>

#include <chrono>
#include <iostream>
#include <thread>  // per std::this_thread::sleep_for

#include "core/Renderer.h"
#include "core/Window.h"
#include "core/Surface.h"
#include "core/Sdl.h"
#include "core/Window.h"
#include "gfx/Texture.h"
#include "game/Game.h"

int main() {
    try {
        core::Sdl sdl; // Inizializza SDL2, SDL_image, SDL_ttf

        core::Window window("Hello SDL", 100, 100, 800, 600);
        core::Renderer renderer(window.get());

        game::Game game;

        game.init();


        /* start Surface example */
        Surface surface(SDL_GetWindowSurface(window.get()));
        SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface.get()->format, 0xFF, 0xFF, 0xFF)); // Riempie lo sfondo di bianco
        SDL_UpdateWindowSurface(window.get());

        /* end Surface example */

        game.mainLoop();
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

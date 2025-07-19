#define SDL_MAIN_HANDLED  // disabilita il wrapping di SDL_main
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <windows.h>

#include <chrono>
#include <iostream>
#include <thread>  // per std::this_thread::sleep_for

#include "core/Renderer.h"
#include "core/Sdl.h"
#include "core/Window.h"
#include "gfx/Texture.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nShowCmd) {
    try {
        core::Sdl sdl; // Inizializza SDL2, SDL_image, SDL_ttf

        core::Window window("Hello SDL", 100, 100, 800, 600);
        core::Renderer renderer(window.get());

        gfx::Texture tex(renderer.get(), "assets/logo.png");
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }
}

#include "Game.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../core/Renderer.h"
#include "../core/Window.h"
#include "state/GameState.h"
#include "state/MenuState.h"
#include "state/PlayingState.h"
#include "state/PausedState.h"

game::Game::Game(core::Window& window)
    : renderer(window),
    state(nullptr),
    inputHandler(nullptr),
    avatar() {
    // Inizializza il renderer OpenGL
}
game::Game::~Game() = default;

void game::Game::init() {
    setState<MenuState>();
    // ... roba varia ...
    inputHandler = std::make_unique<input::InputHandler>();

    inputHandler->loadBindings("data/keybindings.json", avatar);
}

void game::Game::mainLoop() {
    constexpr Uint32 FPS = 60;
    constexpr float FIXED_TIMESTEP = 1.0f / FPS; // 16.67ms
    constexpr float MAX_FRAME_TIME = 0.05f; // 50ms max per evitare spiral of death

    Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    float accumulator = 0.0f;
    bool running = true;

    while (running) {
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float frameTime = static_cast<float>(currentCounter - lastCounter) / static_cast<float>(performanceFrequency);
        lastCounter = currentCounter;

        // Previene spiral of death limitando il frame time massimo
        if (frameTime > MAX_FRAME_TIME) {
            frameTime = MAX_FRAME_TIME;
        }

        accumulator += frameTime;

        // --- input handling ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (state) {
                state->handleEvent(*this, event);
            }
        }

        // --- Fixed timestep updates ---
        while (accumulator >= FIXED_TIMESTEP) {
            if (state) {
                state->update(*this, FIXED_TIMESTEP);
            }
            accumulator -= FIXED_TIMESTEP;
        }

        // --- Rendering ---
        if (state) {
            state->render(*this);
        } else {
            std::cerr << "[Game] No active state to render!\n";
        }

        // --- VSync o frame limiting ---
        // Rimuoviamo SDL_Delay e lasciamo che VSync gestisca il timing
        // Se VSync non è abilitato, potremmo aggiungere un piccolo delay
        SDL_Delay(1); // Piccolo delay per non saturare la CPU
    }
}

core::Renderer& game::Game::getRenderer() {
    return renderer;
}
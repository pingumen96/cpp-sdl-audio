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
    : renderer(window.get()),
    state(nullptr),
    inputHandler(nullptr),
    avatar() {
    // Inizializza il renderer
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
    constexpr Uint32 FRAME_DELAY = 1000 / FPS;
    Uint32 lastTicks = SDL_GetTicks();
    bool running = true;

    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        float dt = (frameStart - lastTicks) / 1000.0f;
        lastTicks = frameStart;

        // --- input handling ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (state) {
                state->handleEvent(*this, event);
            }
        }

        if (state) {
            state->update(*this, dt);
        }

        if (state) {
            state->render(*this);
        } else {
            std::cerr << "[Game] No active state to render!\n";
        }

        // --- frame timing ---
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

core::Renderer& game::Game::getRenderer() {
    return renderer;
}
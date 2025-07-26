#include "Game.h"

#include <iostream>
#include <algorithm>
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
    // Initialize the scene system
    initializeSceneSystem();
}
game::Game::~Game() = default;

void game::Game::init() {
    setState<MenuState>();
    // ... roba varia ...
    inputHandler = std::make_unique<input::InputHandler>();

    inputHandler->loadBindings("data/keybindings.json", avatar);
}

void game::Game::mainLoop() {
    constexpr Uint32 LOGIC_FPS = 60;
    constexpr float FIXED_TIMESTEP = 1.0f / LOGIC_FPS; // 16.67ms per la logica
    constexpr float MAX_FRAME_TIME = 0.05f; // 50ms max per evitare spiral of death

    Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    float accumulator = 0.0f;
    bool running = true;

    while (running) {
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float frameTime = static_cast<float>(currentCounter - lastCounter) / static_cast<float>(performanceFrequency);
        lastCounter = currentCounter;

        // avoid spiral of death by capping frame time
        if (frameTime > MAX_FRAME_TIME) {
            frameTime = MAX_FRAME_TIME;
        }

        accumulator += frameTime;

        // --- INPUT HANDLING ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (state) {
                state->handleEvent(*this, event);
            }
        }

        // --- LOGIC UPDATE (fixed timestep) ---
        bool didUpdate = false;
        while (accumulator >= FIXED_TIMESTEP) {
            if (state) {
                state->update(*this, FIXED_TIMESTEP);
                didUpdate = true;
            }
            accumulator -= FIXED_TIMESTEP;
        }

        // --- RENDERING (variable framerate with interpolation) ---
        if (state && didUpdate) {
            // how much time has passed since the last logical update
            float interpolation = accumulator / FIXED_TIMESTEP;

            // clamp the interpolation between 0 and 1 for safety
            interpolation = std::clamp(interpolation, 0.0f, 1.0f);

            // Traditional state-based rendering
            state->render(*this, interpolation);

            // Additionally, if scene manager is available, update and render scenes
            if (sceneManager) {
                sceneManager->update(FIXED_TIMESTEP);
                sceneManager->render();
            }

            didUpdate = false; // reset for the next cycle
        } else {
            std::cerr << "[Game] No active state to render!\n";
        }

        // little delay to avoid maxing out the CPU
        SDL_Delay(1);
    }
}

core::Renderer& game::Game::getRenderer() {
    return renderer;
}

void game::Game::initializeSceneSystem() {
    // Initialize component type registry
    scene::ComponentTypeRegistry::initializeCommonTypes();

    // Get the OpenGL backend from the renderer
    auto* openGLBackend = renderer.getBackend();
    if (!openGLBackend) {
        std::cerr << "[Game] Failed to get OpenGL backend from renderer!" << std::endl;
        // Fall back to null backend
        sceneManager = scene::createDefaultSceneManager(800, 600);
        return;
    }

    // Since we don't own the backend (it's owned by the renderer), we'll need to use
    // a different approach. Let's create our own copy of the backend specifically for the scene system.
    // For now, let's just use the null backend as a safe fallback.
    sceneManager = scene::createDefaultSceneManager(800, 600);

    if (!sceneManager) {
        std::cerr << "[Game] Failed to create scene manager!" << std::endl;
        return;
    }

    std::cout << "[Game] Scene system initialized successfully (using NullBackend for now)" << std::endl;
}
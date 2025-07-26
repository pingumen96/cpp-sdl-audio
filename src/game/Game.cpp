#include "Game.h"

#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../core/Renderer.h"
#include "../core/Window.h"
// Include new scene-based system instead of old states
#include "MenuScene.h"
#include "GameScene.h"
// Keep old states for transition period
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
    // NEW: Initialize with scene-based system instead of state-based
    if (sceneManager) {
        // Push initial menu scene
        auto menuScene = std::make_unique<MenuScene>();
        sceneManager->pushScene(std::move(menuScene));
        std::cout << "[Game] Started with MenuScene (new system)" << std::endl;
    } else {
        // FALLBACK: Use old state system if scene manager unavailable
        setState<MenuState>();
        std::cout << "[Game] Fallback to old MenuState system" << std::endl;
    }

    // Initialize input handler
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
            } else {
                // NEW: Handle input through scene system
                handleSceneInput(event);

                // FALLBACK: Old state system if scene manager unavailable
                if (!sceneManager && state) {
                    state->handleEvent(*this, event);
                }
            }
        }

        // --- LOGIC UPDATE (fixed timestep) ---
        bool didUpdate = false;
        while (accumulator >= FIXED_TIMESTEP) {
            // PRIMARY: Use scene system for updates
            if (sceneManager) {
                sceneManager->update(FIXED_TIMESTEP);
                didUpdate = true;
            } else if (state) {
                // FALLBACK: Old state system
                state->update(*this, FIXED_TIMESTEP);
                didUpdate = true;
            }
            accumulator -= FIXED_TIMESTEP;
        }

        // --- RENDERING (variable framerate with interpolation) ---
        if (didUpdate) {
            // how much time has passed since the last logical update
            float interpolation = accumulator / FIXED_TIMESTEP;

            // clamp the interpolation between 0 and 1 for safety
            interpolation = std::clamp(interpolation, 0.0f, 1.0f);

            // PRIMARY: Use scene system for rendering
            if (sceneManager) {
                sceneManager->render();
            } else if (state) {
                // FALLBACK: Old state-based rendering
                state->render(*this, interpolation);
            }

            didUpdate = false; // reset for the next cycle
        } else if (!sceneManager) {
            std::cerr << "[Game] No active state/scene to render!\n";
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

void game::Game::handleSceneInput(const SDL_Event& event) {
    if (!sceneManager) return;

    // Get current active scene
    auto* currentScene = sceneManager->getCurrentScene();
    if (!currentScene) return;

    // Handle keyboard input for current scene
    if (event.type == SDL_KEYDOWN) {
        int keyCode = event.key.keysym.sym;

        // Check scene type and forward input appropriately
        if (currentScene->getName() == "MenuScene") {
            // Cast to MenuScene and handle menu input
            if (auto* menuScene = dynamic_cast<MenuScene*>(currentScene)) {
                menuScene->handleMenuInput(keyCode);

                // Handle scene transitions
                if (keyCode == SDLK_RETURN || keyCode == SDLK_KP_ENTER) {
                    // Check if we should transition to game
                    // This is a simplified version - in a real game you'd have better state management
                    auto gameScene = std::make_unique<GameScene>();
                    sceneManager->switchScene(std::move(gameScene));
                }
            }
        } else if (currentScene->getName() == "GameScene") {
            // Cast to GameScene and handle game input
            if (auto* gameScene = dynamic_cast<GameScene*>(currentScene)) {
                gameScene->handleGameInput(keyCode);

                // Handle pause/menu
                if (keyCode == SDLK_ESCAPE) {
                    auto menuScene = std::make_unique<MenuScene>();
                    sceneManager->pushScene(std::move(menuScene)); // Push menu on top
                }
            }
        }
    }
}
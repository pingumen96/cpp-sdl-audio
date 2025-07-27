#include "Game.h"

#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../core/Renderer.h"
#include "../core/Window.h"
// Include unified scene-based system
#include "MenuScene.h"
#include "GameScene.h"

game::Game::Game(core::Window& window)
    : renderer(window),
    inputSystem(nullptr),
    avatar() {
    // Initialize the scene system
    initializeSceneSystem();
}
game::Game::~Game() = default;

void game::Game::init() {
    // Initialize unified scene system
    if (sceneManager) {
        // Push initial menu scene
        auto menuScene = std::make_unique<MenuScene>();
        sceneManager->pushScene(std::move(menuScene));
        std::cout << "[Game] Started with MenuScene" << std::endl;
    } else {
        std::cerr << "[Game] Scene manager not available!" << std::endl;
        return;
    }

    // Initialize unified input system
    inputSystem = std::make_unique<input::SceneInputSystem>();
    inputSystem->loadBindings("data/keybindings.json");

    std::cout << "[Game] Unified input system initialized" << std::endl;
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
                handleInput(event);
            }
        }

        // --- LOGIC UPDATE (fixed timestep) ---
        bool didUpdate = false;
        while (accumulator >= FIXED_TIMESTEP) {
            if (sceneManager) {
                sceneManager->update(FIXED_TIMESTEP);
                didUpdate = true;
            }
            accumulator -= FIXED_TIMESTEP;
        }

        // --- RENDERING (variable framerate) ---
        if (didUpdate && sceneManager) {
            sceneManager->render();
            didUpdate = false;
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

void game::Game::handleInput(const SDL_Event& event) {
    if (!sceneManager || !inputSystem) return;

    // Get current active scene
    auto* currentScene = sceneManager->getCurrentScene();
    if (!currentScene) return;

    // Use unified input system
    bool handled = inputSystem->handleInput(event, currentScene);

    if (!handled && event.type == SDL_KEYDOWN) {
        // Handle built-in scene transitions
        int keyCode = event.key.keysym.sym;

        if (currentScene->getName() == "MenuScene") {
            if (keyCode == SDLK_RETURN || keyCode == SDLK_KP_ENTER) {
                // Transition to game
                auto gameScene = std::make_unique<GameScene>();
                sceneManager->switchScene(std::move(gameScene));
            }
        } else if (currentScene->getName() == "GameScene") {
            if (keyCode == SDLK_ESCAPE) {
                // Return to menu
                auto menuScene = std::make_unique<MenuScene>();
                sceneManager->pushScene(std::move(menuScene));
            }
        }
    }
}
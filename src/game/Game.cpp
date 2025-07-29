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
    avatar() {
    // Initialize the scene system
    initializeSceneSystem();
}
game::Game::~Game() = default;

void game::Game::init() {
    // Initialize unified scene system
    if (sceneManager) {
        // Start with main menu
        auto menuScene = std::make_unique<MenuScene>();
        sceneManager->pushScene(std::move(menuScene));
        std::cout << "[Game] Started with MenuScene" << std::endl;
    } else {
        std::cerr << "[Game] Scene manager not available!" << std::endl;
        return;
    }

    std::cout << "[Game] Initialized successfully" << std::endl;
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
        }

        // Check if any scene has requested quit
        if (sceneManager) {
            auto* currentScene = sceneManager->getCurrentScene();
            if (currentScene && currentScene->getCoordinator()) {
                auto* coord = currentScene->getCoordinator();
                if (coord->hasRuntimeResource<ecs::GlobalFlags>()) {
                    auto& globalFlags = coord->getRuntimeResource<ecs::GlobalFlags>();
                    if (globalFlags.quit) {
                        running = false;
                    }
                }
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

    // Get the render backend from the new architecture and create a dedicated bridge for the scene system
    auto* newBackend = renderer.getBackend();
    if (!newBackend) {
        std::cerr << "[Game] Failed to get render backend from renderer!" << std::endl;
        return;
    }

    // Create a dedicated IRenderBackend bridge for the scene system
    auto sceneBackend = std::make_unique<compat::IRenderBackendBridge>(newBackend);

    // Create scene manager with the backend bridge
    sceneManager = scene::createSceneManager(std::move(sceneBackend), 800, 600);

    if (!sceneManager) {
        std::cerr << "[Game] Failed to create scene manager!" << std::endl;
        return;
    }

    std::cout << "[Game] Scene system initialized successfully with new architecture backend" << std::endl;
}

void game::Game::handleInput(const SDL_Event& event) {
    if (!sceneManager) return;

    // Get current active scene
    auto* currentScene = sceneManager->getCurrentScene();
    if (!currentScene) return;

    // Handle input directly with scene-specific methods
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        int keyCode = event.key.keysym.sym;

        if (currentScene->getName() == "MenuScene") {
            // Cast to MenuScene and handle menu input
            if (auto* menuScene = dynamic_cast<MenuScene*>(currentScene)) {
                if (event.type == SDL_KEYDOWN) {
                    menuScene->handleMenuInput(keyCode);
                }
            }
        }
        // GameScene no longer handles input directly - all input is processed
        // through ECS systems (InputCollectSystem -> InputMappingSystem -> Game Systems)

        // Handle scene transitions (only on keydown)
        if (event.type == SDL_KEYDOWN) {
            if (keyCode == SDLK_RETURN || keyCode == SDLK_KP_ENTER) {
                if (currentScene->getName() == "MenuScene") {
                    // Transition from menu to game
                    auto gameScene = std::make_unique<GameScene>();
                    sceneManager->switchScene(std::move(gameScene));
                }
            } else if (keyCode == SDLK_ESCAPE) {
                if (currentScene->getName() == "GameScene") {
                    // Return to menu from game
                    auto menuScene = std::make_unique<MenuScene>();
                    sceneManager->pushScene(std::move(menuScene));
                }
            }
        }
    }
}
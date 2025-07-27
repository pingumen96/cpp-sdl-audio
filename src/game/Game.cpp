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
#include "PongScene.h"

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
        // Push Pong scene directly for testing
        auto pongScene = std::make_unique<PongScene>();
        sceneManager->pushScene(std::move(pongScene));
        std::cout << "[Game] Started with PongScene directly" << std::endl;
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

    // Create a proper OpenGL backend that wraps the existing renderer
    auto openGLBackend = std::make_unique<core::OpenGLBackend>(renderer.getWindow());

    // Create scene manager with the OpenGL backend
    sceneManager = scene::createSceneManager(std::move(openGLBackend), 800, 600);

    if (!sceneManager) {
        std::cerr << "[Game] Failed to create scene manager!" << std::endl;
        return;
    }

    std::cout << "[Game] Scene system initialized successfully with OpenGL backend" << std::endl;
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
        } else if (currentScene->getName() == "GameScene") {
            // Cast to GameScene and handle game input
            if (auto* gameScene = dynamic_cast<GameScene*>(currentScene)) {
                if (event.type == SDL_KEYDOWN) {
                    gameScene->handleGameInput(keyCode);
                }
            }
        } else if (currentScene->getName() == "PongScene") {
            // Cast to PongScene and handle pong input
            if (auto* pongScene = dynamic_cast<PongScene*>(currentScene)) {
                pongScene->handlePongInput(event);
            }
        }

        // Handle scene transitions (only on keydown)
        if (event.type == SDL_KEYDOWN) {
            if (keyCode == SDLK_RETURN || keyCode == SDLK_KP_ENTER) {
                if (currentScene->getName() == "MenuScene") {
                    // Transition from menu to pong game
                    auto pongScene = std::make_unique<PongScene>();
                    sceneManager->switchScene(std::move(pongScene));
                }
            } else if (keyCode == SDLK_TAB) {
                if (currentScene->getName() == "MenuScene") {
                    // Alternative: switch to regular game scene
                    auto gameScene = std::make_unique<GameScene>();
                    sceneManager->switchScene(std::move(gameScene));
                }
            } else if (keyCode == SDLK_ESCAPE) {
                if (currentScene->getName() == "GameScene" || currentScene->getName() == "PongScene") {
                    // Return to menu from any game
                    auto menuScene = std::make_unique<MenuScene>();
                    sceneManager->pushScene(std::move(menuScene));
                }
            }
        }
    }
}
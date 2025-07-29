#pragma once

#include "SceneManager.h"
#include "rendering/NullBackend.h"
#include <memory>
#include <iostream>

namespace scene {

    /**
     * @brief Example of Renderer2D integration using SceneManager
     *
     * This class demonstrates how to use the centralized SceneManager
     * instead of managing components directly. All rendering functionality
     * is now delegated to SceneManager, eliminating code duplication.
     *
     * NOTE: For ECS integration, use ecs::systems::Renderer2DSystem directly in scenes
     * that inherit from Scene2D.
     */
    class Renderer2DIntegrationExample {
    private:
        // Single dependency - SceneManager manages everything
        std::unique_ptr<SceneManager> sceneManager;

        // State
        uint32_t frameCount = 0;

    public:
        Renderer2DIntegrationExample() = default;
        ~Renderer2DIntegrationExample() = default;

        /**
         * @brief Initialize the entire system using SceneManager
         */
        bool init(uint32_t windowWidth = 1920, uint32_t windowHeight = 1080) {
            if (sceneManager && sceneManager->isInitialized()) {
                return true;
            }

            std::cout << "[Renderer2D Integration] Initializing with SceneManager..." << std::endl;

            // Create SceneManager with NullBackend for testing
            auto backend = std::make_unique<NullBackend>();
            sceneManager = std::make_unique<SceneManager>(std::move(backend));

            if (!sceneManager->initialize(windowWidth, windowHeight)) {
                std::cerr << "[Renderer2D Integration] Failed to initialize SceneManager!" << std::endl;
                return false;
            }

            // Create initial test scene
            createTestScene();

            std::cout << "[Renderer2D Integration] Initialization complete!" << std::endl;
            return true;
        }

        /**
         * @brief Create test scene using SceneManager's 2D facade
         */
        void createTestScene() {
            if (!sceneManager) {
                return;
            }

            // Clear any existing requests
            sceneManager->clear2DRequests();

            // Background
            sceneManager->submit2DQuad(math::Vec2f(0.0f, 0.0f), math::Vec2f(1920.0f, 1080.0f), Color::Blue);

            // Colored rectangles
            sceneManager->submit2DQuad(math::Vec2f(-200.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Red);
            sceneManager->submit2DQuad(math::Vec2f(0.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Green);
            sceneManager->submit2DQuad(math::Vec2f(200.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Yellow);

            // Smaller rectangles
            sceneManager->submit2DQuad(math::Vec2f(-100.0f, 150.0f), math::Vec2f(50.0f, 50.0f), Color::Magenta);
            sceneManager->submit2DQuad(math::Vec2f(100.0f, 150.0f), math::Vec2f(50.0f, 50.0f), Color::Cyan);
        }

        /**
         * @brief Rendering cycle for one frame using SceneManager
         */
        void renderFrame() {
            if (!sceneManager || !sceneManager->isInitialized()) {
                std::cerr << "[Renderer2D Integration] SceneManager not initialized!" << std::endl;
                return;
            }

            frameCount++;

            // Re-create test scene each frame (for demo purposes)
            // In a real application, you would collect draw requests from your game logic
            createTestScene();

            // Render complete frame through SceneManager
            if (!sceneManager->renderFrame()) {
                std::cerr << "[Renderer2D Integration] Failed to render frame!" << std::endl;
                return;
            }

            // Statistics and debug info
            if (frameCount % 60 == 0) { // Every second (assuming 60 FPS)
                printStats();
            }
        }

        /**
         * @brief Shutdown the entire system using SceneManager
         */
        void shutdown() {
            if (!sceneManager) {
                return;
            }

            std::cout << "[Renderer2D Integration] Shutting down..." << std::endl;

            sceneManager->shutdown();
            sceneManager.reset();

            std::cout << "[Renderer2D Integration] Shutdown complete!" << std::endl;
        }

        /**
         * @brief Print rendering statistics using SceneManager
         */
        void printStats() const {
            if (sceneManager) {
                sceneManager->printStats();
            }
        }

        /**
         * @brief Access to components for advanced testing through SceneManager
         */
        IRenderer2D* getRenderer2D() { return sceneManager ? sceneManager->getRenderer2D() : nullptr; }
        Render2DFacade* getFacade() { return sceneManager ? sceneManager->get2DFacade() : nullptr; }
        Camera2D* getTestCamera() { return sceneManager ? &sceneManager->getCamera2D() : nullptr; }
        IRenderBackend* getRenderBackend() { return sceneManager ? sceneManager->getRenderBackend() : nullptr; }
        SceneManager* getSceneManager() { return sceneManager.get(); }

        bool isInitialized() const { return sceneManager && sceneManager->isInitialized(); }
        uint32_t getFrameCount() const { return frameCount; }
    };

    /**
     * @brief Helper function to create and test the system with SceneManager
     */
    inline void runRenderer2DTest(int numFrames = 100) {
        std::cout << "=== Renderer2D Integration Test (SceneManager) ===" << std::endl;

        Renderer2DIntegrationExample example;

        if (!example.init()) {
            std::cerr << "Failed to initialize Renderer2D example!" << std::endl;
            return;
        }

        // Simulate rendering of some frames
        for (int i = 0; i < numFrames; ++i) {
            example.renderFrame();

            // Simulate some scene changes occasionally
            if (i % 30 == 0 && example.getTestCamera()) {
                auto* camera = example.getTestCamera();
                camera->setPosition(math::Vec2f(
                    std::sin(i * 0.1f) * 50.0f,
                    std::cos(i * 0.1f) * 30.0f
                ));
            }
        }

        example.shutdown();
        std::cout << "=== Test Complete ===" << std::endl;
    }

} // namespace scene


#pragma once

#include "rendering/Renderer2DImpl.h"
#include "rendering/NullBackend.h"
#include "rendering/Render2DFacade.h"
#include "../resources/ResourceManager.h"
#include <memory>
#include <iostream>

namespace scene {

    /**
     * @brief Example of Renderer2D integration with existing architecture using unified facade
     *
     * This class demonstrates how to integrate Renderer2D into the existing lifecycle
     * (init → beginFrame → submit → present) without breaking the existing layered approach.
     *
     * Uses the new Render2DFacade instead of TestRenderer2D to eliminate code duplication
     * between ECS and standalone test environments.
     *
     * NOTE: For ECS integration, use ecs::systems::Renderer2DSystem directly in scenes
     * that inherit from Scene2D.
     */
    class Renderer2DIntegrationExample {
    private:
        // External dependencies
        std::unique_ptr<resources::ResourceManager> resourceManager;
        std::unique_ptr<IRenderBackend> renderBackend;

        // 2D components
        std::unique_ptr<RenderQueueBuilder> renderQueueBuilder;
        std::unique_ptr<IRenderer2D> renderer2D;
        std::unique_ptr<Render2DFacade> facade;
        Camera2D testCamera;

        // State
        bool initialized = false;
        uint32_t frameCount = 0;

    public:
        Renderer2DIntegrationExample() = default;
        ~Renderer2DIntegrationExample() = default;

        /**
         * @brief Initialize the entire system
         */
        bool init(uint32_t windowWidth = 1920, uint32_t windowHeight = 1080) {
            if (initialized) {
                return true;
            }

            std::cout << "[Renderer2D Integration] Initializing with unified facade..." << std::endl;

            // 1. Create Resource Manager
            resourceManager = std::make_unique<resources::ResourceManager>();
            // Here you should initialize the ResourceManager with your parameters

            // 2. Create Render Backend (NullBackend for testing)
            renderBackend = std::make_unique<NullBackend>();
            if (!renderBackend->init(windowWidth, windowHeight)) {
                std::cerr << "[Renderer2D Integration] Failed to initialize render backend!" << std::endl;
                return false;
            }

            // 3. Create RenderQueueBuilder
            renderQueueBuilder = std::make_unique<RenderQueueBuilder>();

            // 4. Create Renderer2D
            renderer2D = createRenderer2D(*resourceManager, *renderQueueBuilder);

            RendererConfig2D config;
            config.maxQuadsPerBatch = 1000;
            config.enableBatching = true;
            config.enableSorting = true;

            if (!renderer2D->init(config)) {
                std::cerr << "[Renderer2D Integration] Failed to initialize Renderer2D!" << std::endl;
                return false;
            }

            // 5. Create facade and setup test camera
            facade = std::make_unique<Render2DFacade>();

            // Setup test camera
            testCamera.setViewportSize(math::Vec2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
            testCamera.setPosition(math::Vec2f(0.0f, 0.0f));
            testCamera.setZoom(1.0f);

            // 6. Create test scene
            createTestScene();

            initialized = true;
            std::cout << "[Renderer2D Integration] Initialization complete!" << std::endl;
            return true;
        }

        /**
         * @brief Create test scene (equivalent to TestRenderer2D::createTestScene)
         */
        void createTestScene() {
            if (!facade) {
                return;
            }

            // Clear any existing requests
            facade->clear();

            // Background
            facade->submitColoredQuad(math::Vec2f(0.0f, 0.0f), math::Vec2f(1920.0f, 1080.0f), Color::Blue);

            // Colored rectangles
            facade->submitColoredQuad(math::Vec2f(-200.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Red);
            facade->submitColoredQuad(math::Vec2f(0.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Green);
            facade->submitColoredQuad(math::Vec2f(200.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Yellow);

            // Smaller rectangles
            facade->submitColoredQuad(math::Vec2f(-100.0f, 150.0f), math::Vec2f(50.0f, 50.0f), Color::Magenta);
            facade->submitColoredQuad(math::Vec2f(100.0f, 150.0f), math::Vec2f(50.0f, 50.0f), Color::Cyan);
        }

        /**
         * @brief Rendering cycle for one frame
         */
        void renderFrame() {
            if (!initialized) {
                std::cerr << "[Renderer2D Integration] Not initialized!" << std::endl;
                return;
            }

            frameCount++;

            // === PHASE 1: Begin Frame ===
            if (!renderBackend->beginFrame()) {
                std::cerr << "[Renderer2D Integration] Failed to begin frame!" << std::endl;
                return;
            }

            // === PHASE 2: Clear RenderQueueBuilder ===
            renderQueueBuilder->clear();

            // === PHASE 3: Execute facade rendering ===
            // NOTE: For real ECS integration, use ecs::systems::Renderer2DSystem
            // which is automatically managed by Scene2D
            if (facade && renderer2D) {
                // Re-create test scene each frame (for demo purposes)
                // In a real application, you would collect draw requests from your game logic
                createTestScene();

                // Flush all requests to the renderer
                facade->flush(*renderer2D, testCamera);
            }

            // === PHASE 4: Flush RenderQueueBuilder ===
            // Here you should set up camera and render target
            CameraParams cameraParams;
            // Set up main camera parameters (different from Camera2D)
            // cameraParams.viewMatrix = mainCamera.getViewMatrix();
            // cameraParams.projectionMatrix = mainCamera.getProjectionMatrix();

            RenderTarget target;
            target.width = 1920;
            target.height = 1080;
            target.isBackbuffer = true;

            CommandBuffer commandBuffer = renderQueueBuilder->flush(cameraParams, target);

            // === PHASE 5: Submit to Backend ===
            if (!renderBackend->submit(commandBuffer)) {
                std::cerr << "[Renderer2D Integration] Failed to submit command buffer!" << std::endl;
                return;
            }

            // === PHASE 6: Present ===
            if (!renderBackend->present()) {
                std::cerr << "[Renderer2D Integration] Failed to present frame!" << std::endl;
                return;
            }

            // === PHASE 7: Statistics and debug info ===
            if (frameCount % 60 == 0) { // Every second (assuming 60 FPS)
                printStats();
            }
        }

        /**
         * @brief Shutdown the entire system
         */
        void shutdown() {
            if (!initialized) {
                return;
            }

            std::cout << "[Renderer2D Integration] Shutting down..." << std::endl;

            facade.reset();

            if (renderer2D) {
                renderer2D->shutdown();
                renderer2D.reset();
            }

            renderQueueBuilder.reset();

            if (renderBackend) {
                renderBackend->shutdown();
                renderBackend.reset();
            }

            resourceManager.reset();

            initialized = false;
            std::cout << "[Renderer2D Integration] Shutdown complete!" << std::endl;
        }

        /**
         * @brief Print rendering statistics
         */
        void printStats() const {
            if (!renderer2D || !facade) {
                return;
            }

            auto stats = renderer2D->getStats();
            auto queueSizes = renderQueueBuilder->getQueueSizes();

            std::cout << "[Renderer2D Stats] Frame " << frameCount
                << " - Quads: " << stats.quadCount
                << ", Batches: " << stats.batchCount
                << ", Queue Draw Items: " << queueSizes.drawItems
                << ", Facade Last Frame: " << facade->getLastFrameQuadCount() << std::endl;
        }

        /**
         * @brief Access to components for advanced testing
         */
        IRenderer2D* getRenderer2D() { return renderer2D.get(); }
        Render2DFacade* getFacade() { return facade.get(); }
        Camera2D* getTestCamera() { return &testCamera; }
        IRenderBackend* getRenderBackend() { return renderBackend.get(); }

        bool isInitialized() const { return initialized; }
        uint32_t getFrameCount() const { return frameCount; }
    };

    /**
     * @brief Helper function to create and test the system with unified facade
     */
    inline void runRenderer2DTest(int numFrames = 100) {
        std::cout << "=== Renderer2D Integration Test (Unified Facade) ===" << std::endl;

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

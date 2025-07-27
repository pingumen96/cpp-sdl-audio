#pragma once

#include "rendering/Renderer2DImpl.h"
#include "rendering/NullBackend.h"
#include "TestRenderer2D.h"
#include "../resources/ResourceManager.h"
#include <memory>
#include <iostream>

namespace scene {

    /**
     * @brief Esempio di integrazione del Renderer2D con l'architettura esistente
     *
     * Questa classe dimostra come integrare il Renderer2D nel ciclo di vita
     * esistente (init → beginFrame → submit → present) senza "bucare"
     * la stratificazione già in uso.
     * 
     * NOTA: Per l'integrazione ECS, utilizzare direttamente ecs::systems::Renderer2DSystem
     * nelle scene che ereditano da Scene2D.
     */
    class Renderer2DIntegrationExample {
    private:
        // Dipendenze esterne
        std::unique_ptr<resources::ResourceManager> resourceManager;
        std::unique_ptr<IRenderBackend> renderBackend;

        // Componenti 2D
        std::unique_ptr<RenderQueueBuilder> renderQueueBuilder;
        std::unique_ptr<IRenderer2D> renderer2D;
        std::unique_ptr<TestRenderer2D> testSystem;

        // Stato
        bool initialized = false;
        uint32_t frameCount = 0;

    public:
        Renderer2DIntegrationExample() = default;
        ~Renderer2DIntegrationExample() = default;

        /**
         * @brief Inizializza l'intero sistema
         */
        bool init(uint32_t windowWidth = 1920, uint32_t windowHeight = 1080) {
            if (initialized) {
                return true;
            }

            std::cout << "[Renderer2D Integration] Initializing..." << std::endl;

            // 1. Crea Resource Manager
            resourceManager = std::make_unique<resources::ResourceManager>();
            // Qui dovresti inizializzare il ResourceManager con i tuoi parametri

            // 2. Crea Render Backend (NullBackend per test)
            renderBackend = std::make_unique<NullBackend>();
            if (!renderBackend->init(windowWidth, windowHeight)) {
                std::cerr << "[Renderer2D Integration] Failed to initialize render backend!" << std::endl;
                return false;
            }

            // 3. Crea RenderQueueBuilder
            renderQueueBuilder = std::make_unique<RenderQueueBuilder>();

            // 4. Crea Renderer2D
            renderer2D = createRenderer2D(*resourceManager, *renderQueueBuilder);

            RendererConfig2D config;
            config.maxQuadsPerBatch = 1000;
            config.enableBatching = true;
            config.enableSorting = true;

            if (!renderer2D->init(config)) {
                std::cerr << "[Renderer2D Integration] Failed to initialize Renderer2D!" << std::endl;
                return false;
            }

            // 5. Crea sistema di test
            testSystem = std::make_unique<TestRenderer2D>(*renderer2D);
            testSystem->createTestScene();

            initialized = true;
            std::cout << "[Renderer2D Integration] Initialization complete!" << std::endl;
            return true;
        }

        /**
         * @brief Ciclo di rendering di un frame
         */
        void renderFrame() {
            if (!initialized) {
                std::cerr << "[Renderer2D Integration] Not initialized!" << std::endl;
                return;
            }

            frameCount++;

            // === FASE 1: Begin Frame ===
            if (!renderBackend->beginFrame()) {
                std::cerr << "[Renderer2D Integration] Failed to begin frame!" << std::endl;
                return;
            }

            // === FASE 2: Clear del RenderQueueBuilder ===
            renderQueueBuilder->clear();

            // === FASE 3: Esecuzione del sistema di test ===
            // NOTA: Per l'integrazione ECS reale, utilizzare ecs::systems::Renderer2DSystem
            // che viene gestito automaticamente dalle Scene2D
            testSystem->render();

            // === FASE 4: Flush del RenderQueueBuilder ===
            // Qui dovresti impostare camera e render target
            CameraParams cameraParams;
            // Imposta i parametri della camera principale (diversa dalla Camera2D)
            // cameraParams.viewMatrix = mainCamera.getViewMatrix();
            // cameraParams.projectionMatrix = mainCamera.getProjectionMatrix();

            RenderTarget target;
            target.width = 1920;
            target.height = 1080;
            target.isBackbuffer = true;

            CommandBuffer commandBuffer = renderQueueBuilder->flush(cameraParams, target);

            // === FASE 5: Submit al Backend ===
            if (!renderBackend->submit(commandBuffer)) {
                std::cerr << "[Renderer2D Integration] Failed to submit command buffer!" << std::endl;
                return;
            }

            // === FASE 6: Present ===
            if (!renderBackend->present()) {
                std::cerr << "[Renderer2D Integration] Failed to present frame!" << std::endl;
                return;
            }

            // === FASE 7: Statistiche e debug info ===
            if (frameCount % 60 == 0) { // Ogni secondo (assumendo 60 FPS)
                printStats();
            }
        }

        /**
         * @brief Shutdown dell'intero sistema
         */
        void shutdown() {
            if (!initialized) {
                return;
            }

            std::cout << "[Renderer2D Integration] Shutting down..." << std::endl;

            testSystem.reset();

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
         * @brief Stampa statistiche di rendering
         */
        void printStats() const {
            if (!renderer2D) {
                return;
            }

            auto stats = renderer2D->getStats();
            auto queueSizes = renderQueueBuilder->getQueueSizes();

            std::cout << "[Renderer2D Stats] Frame " << frameCount
                << " - Quads: " << stats.quadCount
                << ", Batches: " << stats.batchCount
                << ", Queue Draw Items: " << queueSizes.drawItems << std::endl;
        }

        /**
         * @brief Accesso ai componenti per test avanzati
         */
        IRenderer2D* getRenderer2D() { return renderer2D.get(); }
        TestRenderer2D* getTestSystem() { return testSystem.get(); }
        IRenderBackend* getRenderBackend() { return renderBackend.get(); }

        bool isInitialized() const { return initialized; }
        uint32_t getFrameCount() const { return frameCount; }
    };

    /**
     * @brief Funzione helper per creare e testare il sistema
     */
    inline void runRenderer2DTest(int numFrames = 100) {
        std::cout << "=== Renderer2D Integration Test ===" << std::endl;

        Renderer2DIntegrationExample example;

        if (!example.init()) {
            std::cerr << "Failed to initialize Renderer2D example!" << std::endl;
            return;
        }

        // Simula il rendering di alcuni frame
        for (int i = 0; i < numFrames; ++i) {
            example.renderFrame();

            // Simula alcune modifiche alla scena ogni tanto
            if (i % 30 == 0 && example.getTestSystem()) {
                auto& camera = example.getTestSystem()->getCamera();
                camera.setPosition(math::Vec2f(
                    std::sin(i * 0.1f) * 50.0f,
                    std::cos(i * 0.1f) * 30.0f
                ));
            }
        }

        example.shutdown();
        std::cout << "=== Test Complete ===" << std::endl;
    }

} // namespace scene

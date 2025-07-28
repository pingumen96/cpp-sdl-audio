#define SDL_MAIN_HANDLED
#include "scene/SceneSystem.h"
#include <iostream>
#include <chrono>
#include <thread>

/**
 * @brief Scene System Demo Application
 *
 * This application demonstrates the complete scene system functionality:
 * - Scene creation and management
 * - Scene transitions
 * - Resource loading
 * - ECS integration
 * - Hierarchical scene nodes
 */
class SceneSystemDemo {
private:
    std::unique_ptr<scene::SceneManager> sceneManager;
    bool running = false;

public:
    /**
     * @brief Initialize the demo
     */
    bool initialize() {
        std::cout << "=== Scene System Demo ===" << std::endl;

        // Initialize component type registry first
        std::cout << "Initializing component type registry..." << std::endl;
        scene::ComponentTypeRegistry::initializeCommonTypes();
        std::cout << "Component types registered: " << scene::ComponentTypeRegistry::getRegisteredTypeCount() << std::endl;

        std::cout << "Initializing scene manager..." << std::endl;

        // Create scene manager with null backend for testing
        sceneManager = scene::createDefaultSceneManager(1920, 1080);
        if (!sceneManager) {
            std::cerr << "Failed to create scene manager!" << std::endl;
            return false;
        }

        std::cout << "Scene manager initialized successfully!" << std::endl;
        std::cout << sceneManager->getDebugInfo() << std::endl;

        return true;
    }

    /**
     * @brief Run the main demo
     */
    void run() {
        running = true;

        std::cout << "\n=== Starting Scene Demo ===" << std::endl;

        // Demonstrate scene operations
        demonstrateBasicSceneOperations();
        demonstrateSceneTransitions();
        demonstrateSceneStack();

        // Run a brief update/render loop
        runUpdateLoop();

        std::cout << "\n=== Demo Complete ===" << std::endl;
    }

    /**
     * @brief Shutdown the demo
     */
    void shutdown() {
        running = false;

        if (sceneManager) {
            std::cout << "\nShutting down scene manager..." << std::endl;
            sceneManager->shutdown();
            sceneManager.reset();
        }

        std::cout << "Demo shutdown complete." << std::endl;
    }

private:
    /**
     * @brief Demonstrate basic scene operations
     */
    void demonstrateBasicSceneOperations() {
        std::cout << "\n--- Basic Scene Operations ---" << std::endl;

        // Create and push a test scene
        auto testScene = std::make_unique<scene::TestScene>();
        std::cout << "Created test scene: " << testScene->getName() << std::endl;

        sceneManager->pushScene(std::move(testScene));
        std::cout << "Pushed test scene to stack" << std::endl;
        std::cout << "Current scene count: " << sceneManager->getSceneCount() << std::endl;

        // Get current scene info
        if (auto* currentScene = sceneManager->getCurrentScene()) {
            std::cout << "Current active scene: " << currentScene->getName()
                << " (WorldID: " << currentScene->getWorldId() << ")" << std::endl;
        }

        // Check resource bundle status
        if (auto* currentScene = sceneManager->getCurrentScene()) {
            bool ready = currentScene->isResourceBundleReady(*sceneManager);
            float progress = currentScene->getResourceBundleProgress(*sceneManager);
            std::cout << "Resource bundle ready: " << (ready ? "Yes" : "No")
                << ", Progress: " << (progress * 100.0f) << "%" << std::endl;
        }
    }

    /**
     * @brief Demonstrate scene transitions
     */
    void demonstrateSceneTransitions() {
        std::cout << "\n--- Scene Transitions ---" << std::endl;

        // Create a menu scene and switch with fade transition
        auto menuScene = std::make_unique<scene::MenuScene>();
        auto fadeTransition = scene::createFadeTransition(0.5f);

        std::cout << "Switching to menu scene with fade transition..." << std::endl;
        sceneManager->switchScene(std::move(menuScene), std::move(fadeTransition));

        // Simulate transition completion
        simulateTransition();

        // Create loading scene with loading transition
        auto loadingScene = std::make_unique<scene::LoadingScene>();
        auto loadingTransition = scene::createLoadingTransition(1.0f, sceneManager->getResourceManager());

        std::cout << "Switching to loading scene with loading transition..." << std::endl;
        sceneManager->switchScene(std::move(loadingScene), std::move(loadingTransition));

        // Simulate loading transition
        simulateTransition();
    }

    /**
     * @brief Demonstrate scene stack management
     */
    void demonstrateSceneStack() {
        std::cout << "\n--- Scene Stack Management ---" << std::endl;

        // Create multiple scenes to demonstrate stack behavior
        auto gameScene = std::make_unique<scene::TestScene>();
        gameScene->setName("GameScene");

        auto overlayScene = std::make_unique<scene::MenuScene>();
        overlayScene->setName("OverlayScene");
        overlayScene->setPausesUnderlying(false); // Don't pause underlying scenes

        // Push game scene
        sceneManager->pushScene(std::move(gameScene));
        std::cout << "Pushed game scene" << std::endl;

        // Push overlay scene
        sceneManager->pushScene(std::move(overlayScene));
        std::cout << "Pushed overlay scene" << std::endl;

        std::cout << "Scene stack:\n" << sceneManager->getDebugInfo() << std::endl;

        // Pop overlay scene
        auto poppedScene = sceneManager->popScene();
        if (poppedScene) {
            std::cout << "Popped scene: " << poppedScene->getName() << std::endl;
        }

        std::cout << "After pop:\n" << sceneManager->getDebugInfo() << std::endl;
    }

    /**
     * @brief Run a brief update/render loop to demonstrate frame processing
     */
    void runUpdateLoop() {
        std::cout << "\n--- Update/Render Loop ---" << std::endl;
        std::cout << "Running 10 frames..." << std::endl;

        const float deltaTime = 1.0f / 60.0f; // 60 FPS

        for (int frame = 0; frame < 10; ++frame) {
            std::cout << "\n=== Frame " << (frame + 1) << " ===" << std::endl;

            // Update scene manager
            sceneManager->update(deltaTime);

            // Render scenes
            bool renderSuccess = sceneManager->render();

            // Show render stats
            auto stats = sceneManager->getLastRenderStats();
            std::cout << "Render Stats - Draw: " << stats.totalDrawItems
                << ", Post: " << stats.totalPostEffects
                << ", UI: " << stats.totalUIItems
                << ", Total Commands: " << stats.commandBufferSize << std::endl;

            if (!renderSuccess) {
                std::cerr << "Render failed on frame " << (frame + 1) << std::endl;
            }

            // Brief pause to make output readable
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Update loop completed!" << std::endl;
    }

    /**
     * @brief Simulate transition completion
     */
    void simulateTransition() {
        if (sceneManager->hasActiveTransition()) {
            std::cout << "Simulating transition: "
                << sceneManager->getCurrentTransition()->getTransitionType() << std::endl;

            // Update transition until complete
            const float deltaTime = 1.0f / 60.0f;
            int frames = 0;

            while (sceneManager->hasActiveTransition() && frames < 120) { // Max 2 seconds
                sceneManager->update(deltaTime);
                frames++;

                if (frames % 30 == 0) { // Print progress every 0.5 seconds
                    auto* transition = sceneManager->getCurrentTransition();
                    if (transition) {
                        std::cout << "Transition progress: "
                            << (transition->getProgress() * 100.0f) << "%" << std::endl;
                    }
                }
            }

            if (sceneManager->hasActiveTransition()) {
                std::cout << "Transition timeout!" << std::endl;
            } else {
                std::cout << "Transition completed!" << std::endl;
            }
        }
    }
};

/**
 * @brief Main entry point for scene system demo
 */
int main() {
    SceneSystemDemo demo;

    try {
        // Initialize demo
        if (!demo.initialize()) {
            std::cerr << "Failed to initialize demo!" << std::endl;
            return 1;
        }

        // Run demo
        demo.run();

        // Shutdown
        demo.shutdown();

        std::cout << "\nDemo completed successfully!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        demo.shutdown();
        return 1;
    }
}

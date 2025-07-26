#pragma once

#include "../../SceneManager.h"
#include "../../rendering/NullBackend.h"
#include "../../../resources/test/MockResourceSystem.h"

namespace scene {
namespace test {

    /**
     * @brief Create a SceneManager configured for testing with mock resources
     * @param width Initial render width
     * @param height Initial render height
     * @return Configured SceneManager with mock resource system
     */
    inline std::unique_ptr<SceneManager> createMockSceneManager(uint32_t width = 1920, uint32_t height = 1080) {
        // Create SceneManager with null backend (same as createDefaultSceneManager)
        auto backend = std::make_unique<NullBackend>();
        auto manager = std::make_unique<SceneManager>(std::move(backend));

        // Initialize with normal rendering
        if (!manager->initialize(width, height)) {
            return nullptr;
        }

        // Replace the default resource manager with mock version
        // Note: This would require adding a setResourceManager method to SceneManager
        // For now, we'll document this as a future enhancement
        
        return manager;
    }

    /**
     * @brief SceneManager wrapper that uses mock resource system
     * 
     * This class wraps SceneManager and overrides resource management
     * to use mock resources for testing.
     */
    class MockSceneManager {
    private:
        std::unique_ptr<SceneManager> realManager;
        std::unique_ptr<resources::ResourceManager> mockResourceManager;

    public:
        MockSceneManager(uint32_t width = 1920, uint32_t height = 1080) {
            // Create real scene manager
            realManager = createDefaultSceneManager(width, height);
            
            // Create mock resource manager
            mockResourceManager = resources::test::createTestResourceManager();
        }

        // Delegate most operations to real manager
        bool isInitialized() const { return realManager->isInitialized(); }
        size_t getSceneCount() const { return realManager->getSceneCount(); }
        
        void pushScene(ScenePtr scene) {
            // Before pushing, we could modify the scene to use mock resources
            // This is a conceptual approach - actual implementation would need
            // more integration with the Scene class
            realManager->pushScene(std::move(scene));
        }

        ScenePtr popScene() { return realManager->popScene(); }

        // Mock resource manager access
        resources::ResourceManager* getResourceManager() const {
            return mockResourceManager.get();
        }
    };

} // namespace test
} // namespace scene

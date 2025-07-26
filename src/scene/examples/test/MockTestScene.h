#pragma once

#include "../../Scene.h"
#include "../../systems/TransformSyncSystem.h"
#include "../../../ecs/components/CommonComponents.h"
#include "../../../resources/test/MockResourceSystem.h"

namespace scene {
namespace test {

    /**
     * @brief Test scene that uses mock resource system
     * 
     * This scene replicates the TestScene behavior but uses mock resources
     * that don't require OpenGL or real file I/O.
     */
    class MockTestScene : public Scene {
    private:
        std::shared_ptr<TransformSyncSystem> transformSystem;

    public:
        /**
         * @brief Constructor
         */
        MockTestScene() : Scene("MockTestScene") {
            // Mock resources are safe to load
        }

        /**
         * @brief Setup scene with mock resources
         */
        void onAttach(SceneManager& manager) override {
            // Register and setup TransformSyncSystem (same as real TestScene)
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
            transformSystem->setCoordinator(coordinator.get());

            ecs::Signature transformSignature;
            transformSignature.set(coordinator->getComponentType<ecs::components::Transform>());
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);

            // Setup mock resource bundle (same files as real TestScene)
            resourceBundle.addResource("data/test.wav");
            resourceBundle.addResource("data/preview.png");

            // Use mock resource manager for loading
            if (auto* resourceManager = getResourceManager(manager)) {
                // This should work because mock loaders handle all file types
                resourceBundle.acquire(*resourceManager, true);
            }
        }

        /**
         * @brief Empty render method for testing
         */
        void render(RenderQueueBuilder& builder) override {
            // Empty render for testing
        }

        /**
         * @brief Get the transform system for testing
         */
        std::shared_ptr<TransformSyncSystem> getTransformSystem() const {
            return transformSystem;
        }

        /**
         * @brief Create a test entity with transform component
         */
        ecs::Entity createTestEntity() {
            ecs::Entity entity = coordinator->createEntity();
            
            ecs::components::Transform transform;
            transform.position = math::Vec3f(1.0f, 2.0f, 3.0f);
            transform.rotation = math::Vec3f(0.0f, 0.0f, 0.0f);
            transform.scale = math::Vec3f(1.0f, 1.0f, 1.0f);
            
            coordinator->addComponent(entity, transform);
            
            return entity;
        }
    };

} // namespace test
} // namespace scene

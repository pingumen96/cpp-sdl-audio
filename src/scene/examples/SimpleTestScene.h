#pragma once

#include "../Scene.h"
#include "../systems/TransformSyncSystem.h"
#include "../../ecs/components/CommonComponents.h"

namespace scene {

    /**
     * @brief Simple test scene that doesn't load any resources
     * 
     * This scene is designed for testing the core scene/ECS functionality
     * without dealing with resource loading complications.
     */
    class SimpleTestScene : public Scene {
    private:
        std::shared_ptr<TransformSyncSystem> transformSystem;

    public:
        /**
         * @brief Constructor
         */
        SimpleTestScene() : Scene("SimpleTestScene") {
            // No resources needed for testing
        }

        /**
         * @brief Setup scene systems without resource loading
         */
        void onAttach(SceneManager& manager) override {
            // Register and setup TransformSyncSystem
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
            transformSystem->setCoordinator(coordinator.get());

            ecs::Signature transformSignature;
            transformSignature.set(coordinator->getComponentType<ecs::components::Transform>());
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);

            // No resource loading - perfect for testing!
            // resourceBundle remains empty
        }

        /**
         * @brief Empty render method for testing
         */
        void render(RenderQueueBuilder& builder) override {
            // Empty render for testing - no actual rendering needed
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

} // namespace scene

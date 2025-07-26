#pragma once

#include "ECS.h"
#include "components/CommonComponents.h"
#include "systems/CommonSystems.h"
#include <iostream>
#include <memory>

namespace ecs {

    /**
     * @brief Example class demonstrating ECS usage
     */
    class ECSExample {
    private:
        std::unique_ptr<Coordinator> mCoordinator;
        std::shared_ptr<systems::PhysicsSystem> mPhysicsSystem;
        std::shared_ptr<systems::RenderSystem> mRenderSystem;
        std::shared_ptr<systems::HealthSystem> mHealthSystem;

    public:
        /**
         * @brief Constructor - initializes ECS and systems
         */
        ECSExample() {
            mCoordinator = createCoordinator();
            initializeECS();
        }

        /**
         * @brief Initialize ECS components and systems
         */
        void initializeECS() {
            // Register component types
            mCoordinator->registerComponent<components::Transform>();
            mCoordinator->registerComponent<components::Velocity>();
            mCoordinator->registerComponent<components::Renderable>();
            mCoordinator->registerComponent<components::Health>();
            mCoordinator->registerComponent<components::PlayerTag>();
            mCoordinator->registerComponent<components::EnemyTag>();

            // Register systems
            mPhysicsSystem = mCoordinator->registerSystem<systems::PhysicsSystem>(mCoordinator.get());
            mRenderSystem = mCoordinator->registerSystem<systems::RenderSystem>(mCoordinator.get());
            mHealthSystem = mCoordinator->registerSystem<systems::HealthSystem>(mCoordinator.get());

            // Set system signatures (which components they require)

            // Physics system needs Transform + Velocity
            Signature physicsSignature;
            physicsSignature.set(mCoordinator->getComponentType<components::Transform>());
            physicsSignature.set(mCoordinator->getComponentType<components::Velocity>());
            mCoordinator->setSystemSignature<systems::PhysicsSystem>(physicsSignature);

            // Render system needs Transform + Renderable
            Signature renderSignature;
            renderSignature.set(mCoordinator->getComponentType<components::Transform>());
            renderSignature.set(mCoordinator->getComponentType<components::Renderable>());
            mCoordinator->setSystemSignature<systems::RenderSystem>(renderSignature);

            // Health system only needs Health component
            Signature healthSignature;
            healthSignature.set(mCoordinator->getComponentType<components::Health>());
            mCoordinator->setSystemSignature<systems::HealthSystem>(healthSignature);
        }

        /**
         * @brief Create a player entity with all necessary components
         * @return Entity ID of the created player
         */
        Entity createPlayer() {
            Entity player = mCoordinator->createEntity();

            // Add components
            mCoordinator->addComponent(player, components::Transform{
                math::Vec3f{0.0f, 0.0f, 0.0f}  // Starting position
                });

            mCoordinator->addComponent(player, components::Velocity{
                math::Vec3f{0.0f, 0.0f, 0.0f}  // Initially at rest
                });

            mCoordinator->addComponent(player, components::Renderable{
                math::Vec4f{0.0f, 1.0f, 0.0f, 1.0f}  // Green color
                });

            mCoordinator->addComponent(player, components::Health{ 100.0f });

            mCoordinator->addComponent(player, components::PlayerTag{});

            std::cout << "[ECSExample] Created player entity: " << player << std::endl;
            return player;
        }

        /**
         * @brief Create an enemy entity
         * @return Entity ID of the created enemy
         */
        Entity createEnemy(const math::Vec3f& position) {
            Entity enemy = mCoordinator->createEntity();

            mCoordinator->addComponent(enemy, components::Transform{ position });

            mCoordinator->addComponent(enemy, components::Velocity{
                math::Vec3f{1.0f, 0.0f, 0.0f}  // Moving right
                });

            mCoordinator->addComponent(enemy, components::Renderable{
                math::Vec4f{1.0f, 0.0f, 0.0f, 1.0f}  // Red color
                });

            mCoordinator->addComponent(enemy, components::Health{ 50.0f });

            mCoordinator->addComponent(enemy, components::EnemyTag{});

            std::cout << "[ECSExample] Created enemy entity: " << enemy << " at position ("
                << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
            return enemy;
        }

        /**
         * @brief Create a decorative object (only visual, no physics or health)
         * @return Entity ID of the created decoration
         */
        Entity createDecoration(const math::Vec3f& position) {
            Entity decoration = mCoordinator->createEntity();

            mCoordinator->addComponent(decoration, components::Transform{ position });

            mCoordinator->addComponent(decoration, components::Renderable{
                math::Vec4f{0.0f, 0.0f, 1.0f, 0.5f}  // Semi-transparent blue
                });

            std::cout << "[ECSExample] Created decoration entity: " << decoration << std::endl;
            return decoration;
        }

        /**
         * @brief Update all systems
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) {
            mCoordinator->updateSystems(deltaTime);
        }

        /**
         * @brief Get the ECS coordinator
         * @return Reference to the coordinator
         */
        Coordinator& getCoordinator() {
            return *mCoordinator;
        }

        /**
         * @brief Get the health system for external damage/healing operations
         * @return Shared pointer to health system
         */
        std::shared_ptr<systems::HealthSystem> getHealthSystem() {
            return mHealthSystem;
        }

        /**
         * @brief Display ECS statistics
         */
        void displayStats() {
            std::cout << "\n=== ECS Statistics ===" << std::endl;
            std::cout << "Living entities: " << mCoordinator->getLivingEntityCount() << std::endl;
            std::cout << "Registered components: " << mCoordinator->getRegisteredComponentCount() << std::endl;
            std::cout << "Registered systems: " << mCoordinator->getSystemCount() << std::endl;

            std::cout << "\nComponent counts:" << std::endl;
            std::cout << "  Transform: " << mCoordinator->getComponentCount<components::Transform>() << std::endl;
            std::cout << "  Velocity: " << mCoordinator->getComponentCount<components::Velocity>() << std::endl;
            std::cout << "  Renderable: " << mCoordinator->getComponentCount<components::Renderable>() << std::endl;
            std::cout << "  Health: " << mCoordinator->getComponentCount<components::Health>() << std::endl;

            std::cout << "\nSystem entity counts:" << std::endl;
            std::cout << "  Physics: " << mPhysicsSystem->getEntityCount() << std::endl;
            std::cout << "  Render: " << mRenderSystem->getEntityCount() << std::endl;
            std::cout << "  Health: " << mHealthSystem->getEntityCount() << std::endl;
        }

        /**
         * @brief Run a complete demonstration of the ECS
         */
        void runDemo() {
            std::cout << "\n=== ECS Demo Starting ===" << std::endl;

            // Create some entities
            Entity player = createPlayer();
            Entity enemy1 = createEnemy(math::Vec3f{ 10.0f, 0.0f, 0.0f });
            Entity enemy2 = createEnemy(math::Vec3f{ -5.0f, 3.0f, 0.0f });
            Entity decoration = createDecoration(math::Vec3f{ 0.0f, 5.0f, 0.0f });

            displayStats();

            // Simulate some game time
            std::cout << "\n=== Simulating game updates ===" << std::endl;
            for (int frame = 0; frame < 3; ++frame) {
                std::cout << "\n--- Frame " << (frame + 1) << " ---" << std::endl;
                update(0.016f); // 60 FPS
            }

            // Test damage system
            std::cout << "\n=== Testing damage system ===" << std::endl;
            mHealthSystem->damageEntity(enemy1, 30.0f);
            mHealthSystem->damageEntity(enemy2, 60.0f); // This should kill enemy2

            std::cout << "\n--- Update after damage ---" << std::endl;
            update(0.016f);

            displayStats();

            std::cout << "\n=== ECS Demo Complete ===" << std::endl;
        }
    };

} // namespace ecs

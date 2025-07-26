#pragma once

#include "../System.h"
#include "../Coordinator.h"
#include "../components/CommonComponents.h"
#include <iostream>

namespace ecs::systems {

    /**
     * @brief Physics system that updates entity positions based on velocity
     */
    class PhysicsSystem : public System {
    private:
        Coordinator* mCoordinator;

    public:
        /**
         * @brief Constructor
         * @param coordinator Pointer to ECS coordinator
         */
        explicit PhysicsSystem(Coordinator* coordinator)
            : mCoordinator(coordinator) {}

        /**
         * @brief Updates physics for all entities with Transform and Velocity components
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override {
            for (auto const& entity : mEntities) {
                auto& transform = mCoordinator->getComponent<components::Transform>(entity);
                auto& velocity = mCoordinator->getComponent<components::Velocity>(entity);

                // Update position based on linear velocity
                transform.position = transform.position + velocity.linear * deltaTime;

                // Update rotation based on angular velocity (quaternion integration)
                if (velocity.angular.length() > 0.0f) {
                    float angle = velocity.angular.length() * deltaTime;
                    math::Vec3f axis = velocity.angular.normalized();
                    math::Quatf deltaRotation(angle, axis);
                    transform.rotation = transform.rotation * deltaRotation;
                }
            }
        }
    };

    /**
     * @brief Render system that handles visual rendering of entities
     */
    class RenderSystem : public System {
    private:
        Coordinator* mCoordinator;

    public:
        /**
         * @brief Constructor
         * @param coordinator Pointer to ECS coordinator
         */
        explicit RenderSystem(Coordinator* coordinator)
            : mCoordinator(coordinator) {}

        /**
         * @brief Renders all entities with Transform and Renderable components
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override {
            // In a real implementation, this would interface with the graphics system
            // For demonstration purposes, we'll just count visible entities

            size_t visibleCount = 0;
            for (auto const& entity : mEntities) {
                auto& renderable = mCoordinator->getComponent<components::Renderable>(entity);

                if (renderable.visible) {
                    visibleCount++;
                    // Here you would submit the entity for rendering
                    // auto& transform = mCoordinator->getComponent<components::Transform>(entity);
                    // renderEntity(entity, transform, renderable);
                }
            }

            // Debug output (remove in production)
            if (visibleCount > 0) {
                std::cout << "[RenderSystem] Rendering " << visibleCount << " entities" << std::endl;
            }
        }
    };

    /**
     * @brief Health system that manages entity health and death
     */
    class HealthSystem : public System {
    private:
        Coordinator* mCoordinator;
        std::vector<Entity> mEntitiesToDestroy; // Entities to be destroyed this frame

    public:
        /**
         * @brief Constructor
         * @param coordinator Pointer to ECS coordinator
         */
        explicit HealthSystem(Coordinator* coordinator)
            : mCoordinator(coordinator) {}

        /**
         * @brief Updates health status and handles entity death
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override {
            mEntitiesToDestroy.clear();

            for (auto const& entity : mEntities) {
                auto& health = mCoordinator->getComponent<components::Health>(entity);

                // Check if entity should be destroyed
                if (!health.isAlive()) {
                    mEntitiesToDestroy.push_back(entity);
                }
            }

            // Destroy dead entities
            for (auto entity : mEntitiesToDestroy) {
                std::cout << "[HealthSystem] Entity " << entity << " died and will be destroyed" << std::endl;
                mCoordinator->destroyEntity(entity);
            }
        }

        /**
         * @brief Apply damage to an entity
         * @param entity Target entity
         * @param damage Damage amount
         */
        void damageEntity(Entity entity, float damage) {
            if (mCoordinator->hasComponent<components::Health>(entity)) {
                auto& health = mCoordinator->getComponent<components::Health>(entity);
                health.takeDamage(damage);

                std::cout << "[HealthSystem] Entity " << entity << " took " << damage
                    << " damage. Health: " << health.current << "/" << health.maximum << std::endl;
            }
        }

        /**
         * @brief Heal an entity
         * @param entity Target entity
         * @param amount Healing amount
         */
        void healEntity(Entity entity, float amount) {
            if (mCoordinator->hasComponent<components::Health>(entity)) {
                auto& health = mCoordinator->getComponent<components::Health>(entity);
                health.heal(amount);

                std::cout << "[HealthSystem] Entity " << entity << " healed " << amount
                    << " points. Health: " << health.current << "/" << health.maximum << std::endl;
            }
        }
    };

} // namespace ecs::systems

// Include additional system types
#include "Renderer2DSystem.h"

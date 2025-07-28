#pragma once

#include "../ecs/System.h"
#include "../ecs/Coordinator.h"
#include "../ecs/components/CommonComponents.h"
#include "../ecs/InputState.h"
#include "../ecs/EventBus.h"
#include "../ecs/events/InputEvents.h"

namespace game {

    /**
     * @brief Example system for player movement logic
     *
     * This system demonstrates the ECS pattern:
     * 1. Reads input events from InputMappingSystem
     * 2. Updates player components (Transform, Velocity)
     * 3. Handles physics integration
     */
    class PlayerMovementSystem : public ecs::System {
    private:
        float moveSpeed = 200.0f;
        float jumpForce = 400.0f;
        bool isGrounded = true; // Simplified for example

    public:
        void update(float deltaTime) override {
            auto* coordinator = getCoordinator();
            if (!coordinator) return;

            // Example: Process movement intents from InputMappingSystem
            // In a real implementation, these would come from events

            for (auto entity : entities) {
                // Get required components
                auto& transform = coordinator->getComponent<ecs::components::Transform>(entity);
                auto& velocity = coordinator->getComponent<ecs::components::Velocity>(entity);

                // Apply movement (this would normally come from input events)
                // velocity.linear.x() = moveSpeed * inputDirection.x;

                // Apply physics integration
                transform.position.x() += velocity.linear.x() * deltaTime;
                transform.position.y() += velocity.linear.y() * deltaTime;

                // Apply gravity (example)
                if (!isGrounded) {
                    velocity.linear.y() -= 980.0f * deltaTime; // gravity
                }

                // Keep player in bounds
                transform.position.x() = std::max(0.0f, std::min(768.0f, transform.position.x()));
                transform.position.y() = std::max(0.0f, std::min(568.0f, transform.position.y()));
            }
        }

        void handleEvent(const ecs::events::BaseEvent& event) override {
            // Handle movement intents from InputMappingSystem
            // if (auto* moveEvent = dynamic_cast<const MoveIntentEvent*>(&event)) {
            //     // Apply movement based on intent
            // }
        }
    };

    /**
     * @brief Example system for enemy AI behavior
     */
    class EnemyAISystem : public ecs::System {
    public:
        void update(float deltaTime) override {
            auto* coordinator = getCoordinator();
            if (!coordinator) return;

            for (auto entity : entities) {
                auto& transform = coordinator->getComponent<ecs::components::Transform>(entity);
                auto& velocity = coordinator->getComponent<ecs::components::Velocity>(entity);

                // Simple AI: move back and forth
                static float direction = 1.0f;
                velocity.linear.x() = 50.0f * direction;

                // Bounce at screen edges
                if (transform.position.x() <= 0.0f || transform.position.x() >= 768.0f) {
                    direction *= -1.0f;
                }

                // Apply movement
                transform.position.x() += velocity.linear.x() * deltaTime;
            }
        }
    };

    /**
     * @brief Example system for collision detection and response
     */
    class CollisionSystem : public ecs::System {
    public:
        void update(float deltaTime) override {
            auto* coordinator = getCoordinator();
            if (!coordinator) return;

            // Example: Check collisions between different entity types
            // This would normally use spatial partitioning for performance

            std::vector<ecs::Entity> players;
            std::vector<ecs::Entity> enemies;

            // Categorize entities (simplified example)
            for (auto entity : entities) {
                if (coordinator->hasComponent<ecs::components::PlayerTag>(entity)) {
                    players.push_back(entity);
                } else if (coordinator->hasComponent<ecs::components::EnemyTag>(entity)) {
                    enemies.push_back(entity);
                }
            }

            // Check player-enemy collisions
            for (auto player : players) {
                auto& playerTransform = coordinator->getComponent<ecs::components::Transform>(player);

                for (auto enemy : enemies) {
                    auto& enemyTransform = coordinator->getComponent<ecs::components::Transform>(enemy);

                    // Simple AABB collision
                    float distance = std::abs(playerTransform.position.x() - enemyTransform.position.x()) +
                        std::abs(playerTransform.position.y() - enemyTransform.position.y());

                    if (distance < 50.0f) { // Collision threshold
                        // Handle collision - could emit events, damage player, etc.
                        // coordinator->getComponent<ecs::components::Health>(player).current -= 10;
                    }
                }
            }
        }
    };

    /**
     * @brief Example system for game rules and win/lose conditions
     */
    class GameRuleSystem : public ecs::System {
    private:
        int score = 0;
        int lives = 3;

    public:
        void update(float deltaTime) override {
            auto* coordinator = getCoordinator();
            if (!coordinator) return;

            // Check win/lose conditions
            for (auto entity : entities) {
                if (coordinator->hasComponent<ecs::components::PlayerTag>(entity)) {
                    auto& health = coordinator->getComponent<ecs::components::Health>(entity);

                    if (health.current <= 0) {
                        lives--;
                        health.current = health.maximum; // Respawn

                        if (lives <= 0) {
                            // Game Over
                            std::cout << "[GameRuleSystem] Game Over! Final Score: " << score << std::endl;
                        }
                    }
                }
            }
        }

        void handleEvent(const ecs::events::BaseEvent& event) override {
            // Handle scoring events, power-ups, etc.
        }
    };

} // namespace game

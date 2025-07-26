#pragma once

#include "../scene/Scene2D.h"
#include "../math/math.h"

namespace game {

    /**
     * @brief Main game scene using unified ECS+Renderer2D architecture
     *
     * This scene replaces parts of PlayingState and demonstrates how
     * game entities are managed through ECS with Renderable2D components.
     */
    class GameScene : public scene::Scene2D {
    private:
        // Game entities
        ecs::Entity playerEntity = 0;
        ecs::Entity backgroundEntity = 0;
        std::vector<ecs::Entity> gameEntities;

    public:
        GameScene() {
            setName("GameScene");
            setPausesUnderlying(true); // Game pauses menu underneath
        }

        void onAttach(scene::SceneManager& manager) override {
            std::cout << "[GameScene] Game scene attached" << std::endl;

            // Initialize 2D rendering system
            initialize2D(manager);

            // Set up game resources
            auto& resourceBundle = getResourceBundle();
            resourceBundle.addResource("textures/player.png");
            resourceBundle.addResource("textures/background.png");
            resourceBundle.addResource("textures/enemy.png");

            // Create game entities
            createGameEntities();
        }

        void onActivate() override {
            std::cout << "[GameScene] Game activated" << std::endl;
        }

        void onDeactivate() override {
            std::cout << "[GameScene] Game deactivated" << std::endl;
        }

        void onDetach(scene::SceneManager& manager) override {
            std::cout << "[GameScene] Game scene detached" << std::endl;
            scene::Scene2D::onDetach(manager);
        }

        void update(float deltaTime) override {
            // Update ECS systems (physics, health, rendering)
            scene::Scene2D::update(deltaTime);

            // Game-specific logic
            updateGameLogic(deltaTime);
        }

        /**
         * @brief Handle game input (to be called from Game class)
         */
        void handleGameInput(int keyCode) {
            auto* coordinator = getCoordinator();
            if (!coordinator || playerEntity == 0) return;

            // Get player transform
            auto& transform = coordinator->getComponent<ecs::components::Transform>(playerEntity);

            const float moveSpeed = 200.0f; // pixels per second

            switch (keyCode) {
            case SDLK_w:
            case SDLK_UP:
                transform.position.y() -= moveSpeed * 0.016f; // Approximate deltaTime
                break;
            case SDLK_s:
            case SDLK_DOWN:
                transform.position.y() += moveSpeed * 0.016f;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                transform.position.x() -= moveSpeed * 0.016f;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                transform.position.x() += moveSpeed * 0.016f;
                break;
            }

            // Keep player in bounds
            transform.position.x() = std::max(0.0f, std::min(750.0f, transform.position.x()));
            transform.position.y() = std::max(0.0f, std::min(550.0f, transform.position.y()));
        }

    private:
        /**
         * @brief Create game entities using ECS
         */
        void createGameEntities() {
            // Create background
            backgroundEntity = createColoredQuad(
                math::Vec2f(0.0f, 0.0f),
                math::Vec2f(800.0f, 600.0f),
                scene::Color(0.1f, 0.3f, 0.1f, 1.0f), // Dark green background
                0 // Background layer
            );

            // Create player entity
            playerEntity = createSprite(
                math::Vec2f(400.0f, 300.0f), // Center of screen
                math::Vec2f(50.0f, 50.0f),   // 50x50 pixels
                "player",                     // Texture ID
                scene::Color::White,          // Tint
                5                            // Player layer
            );

            // Add additional components to player
            auto* coordinator = getCoordinator();
            if (coordinator) {
                // Add health
                ecs::components::Health health;
                health.maximum = 100;
                health.current = 100;
                coordinator->addComponent(playerEntity, health);

                // Add player tag
                coordinator->addComponent(playerEntity, ecs::components::PlayerTag{});

                // Add velocity for physics
                ecs::components::Velocity velocity;
                coordinator->addComponent(playerEntity, velocity);
            }

            // Create some test enemies
            createTestEnemies();
        }

        /**
         * @brief Create test enemy entities
         */
        void createTestEnemies() {
            for (int i = 0; i < 3; ++i) {
                float x = 100.0f + i * 200.0f;
                float y = 100.0f;

                ecs::Entity enemy = createSprite(
                    math::Vec2f(x, y),
                    math::Vec2f(40.0f, 40.0f),
                    "enemy",
                    scene::Color(1.0f, 0.3f, 0.3f, 1.0f), // Red tint
                    4 // Enemy layer
                );

                auto* coordinator = getCoordinator();
                if (coordinator) {
                    // Add health
                    ecs::components::Health health;
                    health.maximum = 50;
                    health.current = 50;
                    coordinator->addComponent(enemy, health);

                    // Add enemy tag
                    coordinator->addComponent(enemy, ecs::components::EnemyTag{});

                    // Add velocity for movement
                    ecs::components::Velocity velocity;
                    velocity.linear = math::Vec3f(0.0f, 50.0f, 0.0f); // Move down slowly
                    coordinator->addComponent(enemy, velocity);
                }

                gameEntities.push_back(enemy);
            }
        }

        /**
         * @brief Game-specific update logic
         */
        void updateGameLogic(float deltaTime) {
            auto* coordinator = getCoordinator();
            if (!coordinator) return;

            // Example: Make enemies bounce at screen edges
            for (auto enemy : gameEntities) {
                auto& transform = coordinator->getComponent<ecs::components::Transform>(enemy);
                auto& velocity = coordinator->getComponent<ecs::components::Velocity>(enemy);

                // Bounce at screen edges
                if (transform.position.y() > 550.0f || transform.position.y() < 50.0f) {
                    velocity.linear.y() *= -1.0f;
                }
            }
        }

        /**
         * @brief Custom 2D rendering for game-specific effects
         */
        void render2DCustom() override {
            // Add custom rendering here (HUD, particles, effects, etc.)
            // Game entities are automatically rendered by the ECS system

            // Example: Render a simple HUD
            if (auto* renderer = getRenderer2D()) {
                // HUD background
                scene::Rect2D hudRect;
                hudRect.position = math::Vec2f(10.0f, 10.0f);
                hudRect.size = math::Vec2f(200.0f, 50.0f);

                renderer->drawRect(
                    hudRect,
                    scene::Color(0.0f, 0.0f, 0.0f, 0.7f) // Semi-transparent black
                );
            }
        }
    };

} // namespace game

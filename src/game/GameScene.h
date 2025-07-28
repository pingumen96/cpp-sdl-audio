#pragma once

#include "../scene/Scene2D.h"
#include "../math/math.h"

namespace game {

    /**
     * @brief Main game scene using pure ECS architecture
     *
     * This scene serves as a factory for entities and resources, then delegates
     * all game logic to dedicated ECS systems. It follows the principle:
     * - Scene = Entity/Resource factory + System registration
     * - Systems = All game logic and frame-by-frame updates
     * - Components = Pure data containers
     */
    class GameScene : public scene::Scene2D {
    private:
        // Optional: Keep references to key entities for specific operations (respawn, etc.)
        // If not needed, these can be removed entirely
        ecs::Entity backgroundEntity = 0;

    public:
        GameScene() {
            setName("GameScene");
            setPausesUnderlying(true); // Game pauses menu underneath
        }

        void onAttach(scene::SceneManager& manager) override {
            std::cout << "[GameScene] Game scene attached" << std::endl;

            // Initialize 2D rendering system
            initialize2D(manager);

            // Set up game resources - add your textures and sounds here
            auto& resourceBundle = getResourceBundle();
            // resourceBundle.addResource("textures/player.png");
            // resourceBundle.addResource("textures/enemy.png");
            // resourceBundle.addResource("sounds/jump.wav");
            // resourceBundle.addResource("sounds/collect.wav");

            // --- Register systems specific to this scene ---
            // These systems handle all game logic and frame-by-frame updates
            // manager.addSystem<PlayerMovementSystem>();
            // manager.addSystem<EnemyAISystem>();
            // manager.addSystem<CollisionSystem>();
            // manager.addSystem<GameRuleSystem>();
            // manager.addSystem<ParticleSystem>();

            // Create initial game entities (pure factory pattern)
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

            // --- Unregister scene-specific systems ---
            // Clean up systems registered in onAttach
            // manager.removeSystem<PlayerMovementSystem>();
            // manager.removeSystem<EnemyAISystem>();
            // manager.removeSystem<CollisionSystem>();
            // manager.removeSystem<GameRuleSystem>();
            // manager.removeSystem<ParticleSystem>();

            scene::Scene2D::onDetach(manager);
        }

        void update(float deltaTime) override {
            // Pure ECS approach: scene only updates base systems
            // All game logic is handled by registered systems
            scene::Scene2D::update(deltaTime);

            // No game logic here! Systems handle everything:
            // - InputMappingSystem converts input to intents
            // - PlayerMovementSystem handles movement
            // - CollisionSystem handles collisions
            // - GameRuleSystem handles scoring/win conditions
            // - etc.
        }

    private:
        /**
         * @brief Create initial game entities (pure factory pattern)
         *
         * This method only creates entities and assigns initial components.
         * All behavior and logic is handled by systems, not by the scene.
         */
        void createGameEntities() {
            // Create background entity
            backgroundEntity = createColoredQuad(
                math::Vec2f(0.0f, 0.0f),
                math::Vec2f(800.0f, 600.0f),
                scene::Color(0.1f, 0.1f, 0.3f, 1.0f), // Dark blue background
                0 // Background layer
            );

            // Example: Create player entity
            // auto* coordinator = getCoordinator();
            // if (coordinator) {
            //     ecs::Entity playerEntity = createSprite(
            //         math::Vec2f(100.0f, 300.0f),    // Starting position
            //         math::Vec2f(32.0f, 32.0f),      // Size
            //         "player",                        // Texture ID
            //         scene::Color::White,             // Tint
            //         5                               // Layer
            //     );
            //
            //     // Add gameplay components
            //     coordinator->addComponent(playerEntity, ecs::components::PlayerTag{});
            //     coordinator->addComponent(playerEntity, ecs::components::Health{100, 100});
            //     coordinator->addComponent(playerEntity, ecs::components::Velocity{});
            //
            //     // Movement-related components
            //     PlayerMovementComponent movement{};
            //     movement.speed = 200.0f;
            //     movement.jumpForce = 400.0f;
            //     coordinator->addComponent(playerEntity, movement);
            // }

            // Example: Create enemies, collectibles, etc.
            // for (int i = 0; i < 3; ++i) {
            //     ecs::Entity enemy = createSprite(...);
            //     coordinator->addComponent(enemy, ecs::components::EnemyTag{});
            //     coordinator->addComponent(enemy, EnemyAIComponent{});
            // }
        }

        /**
         * @brief Custom 2D rendering for scene-specific effects
         *
         * Use this for effects that don't belong to any entity:
         * - Screen-wide shaders
         * - UI overlays that aren't entities
         * - Debug visualization
         */
        void render2DCustom() override {
            // Example: Render debug information
            // auto* renderer = getRenderer2D();
            // if (renderer) {
            //     // Draw UI that isn't entity-based
            //     scene::Rect2D debugRect;
            //     debugRect.position = math::Vec2f(10.0f, 10.0f);
            //     debugRect.size = math::Vec2f(200.0f, 50.0f);
            //     renderer->drawRect(debugRect, scene::Color(0.0f, 0.0f, 0.0f, 0.5f));
            // }
        }
    };

} // namespace game

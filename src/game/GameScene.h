#pragma once

#include "../scene/Scene2D.h"
#include "../math/math.h"

namespace game {

    /**
     * @brief Main game scene using unified ECS+Renderer2D architecture
     *
     * This is a clean template for implementing a new game.
     * Game entities are managed through ECS with Renderable2D components.
     */
    class GameScene : public scene::Scene2D {
    private:
        // Game entities - add your game-specific entities here
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

            // Set up game resources - add your textures and sounds here
            auto& resourceBundle = getResourceBundle();
            // resourceBundle.addResource("textures/your_texture.png");
            // resourceBundle.addResource("sounds/your_sound.wav");

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
            // Update base scene systems
            scene::Scene2D::update(deltaTime);

            // Update game-specific logic
            updateGameLogic(deltaTime);
        }

        /**
         * @brief Handle game input
         */
        void handleGameInput(int keyCode) {
            // Implement your game input handling here
            // Example:
            // switch (keyCode) {
            //     case SDLK_SPACE:
            //         // Handle spacebar
            //         break;
            //     // Add more keys as needed
            // }
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
                scene::Color(0.1f, 0.1f, 0.3f, 1.0f), // Dark blue background
                0 // Background layer
            );

            // Add your game entities here
            // Example:
            // ecs::Entity playerEntity = createSprite(
            //     math::Vec2f(400.0f, 300.0f), // Position
            //     math::Vec2f(50.0f, 50.0f),   // Size
            //     "player",                     // Texture ID
            //     scene::Color::White,          // Tint
            //     5                            // Layer
            // );
        }

        /**
         * @brief Game-specific update logic
         */
        void updateGameLogic(float deltaTime) {
            // Implement your game logic here
            // Example: move entities, check collisions, update AI, etc.
            
            // auto* coordinator = getCoordinator();
            // if (!coordinator) return;
            
            // Update game entities as needed
        }

        /**
         * @brief Custom 2D rendering for game-specific effects
         */
        void render2DCustom() override {
            // Add any custom rendering here (optional)
            // This is called after all entities are rendered
        }
    };

} // namespace game

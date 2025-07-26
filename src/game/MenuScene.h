#pragma once

#include "../scene/Scene2D.h"
#include "../math/math.h"
#include <vector>
#include <string>

namespace game {

    /**
     * @brief Main menu scene using unified ECS+Renderer2D architecture
     *
     * This scene replaces the old MenuState and demonstrates the new
     * unified approach where:
     * - Menu items are ECS entities with Renderable2D components
     * - Input handling creates/modifies ECS entities
     * - All rendering goes through Renderer2D system
     */
    class MenuScene : public scene::Scene2D {
    private:
        // Menu state
        std::vector<std::string> menuEntries{ "Start", "Options", "Quit" };
        int selectedIndex = 0;

        // ECS entities for menu items
        std::vector<ecs::Entity> menuItemEntities;
        ecs::Entity backgroundEntity = 0;
        ecs::Entity titleEntity = 0;

        // Menu layout
        static constexpr float MENU_START_Y = 300.0f;
        static constexpr float MENU_ITEM_HEIGHT = 60.0f;
        static constexpr float MENU_ITEM_WIDTH = 200.0f;
        static constexpr float MENU_CENTER_X = 400.0f;

    public:
        MenuScene() {
            setName("MenuScene");
            setPausesUnderlying(true); // Menu pauses underlying scenes
        }

        void onAttach(scene::SceneManager& manager) override {
            std::cout << "[MenuScene] Scene attached" << std::endl;

            // Initialize 2D rendering system
            initialize2D(manager);

            // Setup menu resources
            auto& resourceBundle = getResourceBundle();
            resourceBundle.addResource("textures/menu_background.png");
            resourceBundle.addResource("textures/menu_button.png");
            resourceBundle.addResource("textures/menu_button_selected.png");

            // Create menu entities
            createMenuEntities();
        }

        void onActivate() override {
            std::cout << "[MenuScene] Menu activated" << std::endl;
        }

        void onDeactivate() override {
            std::cout << "[MenuScene] Menu deactivated" << std::endl;
        }

        void onDetach(scene::SceneManager& manager) override {
            std::cout << "[MenuScene] Menu detached" << std::endl;
            scene::Scene2D::onDetach(manager);
        }

        void update(float deltaTime) override {
            // Update ECS systems (this handles entity updates)
            scene::Scene2D::update(deltaTime);

            // Handle menu selection visual updates
            updateMenuSelection();
        }

        /**
         * @brief Handle input events (to be called from Game class)
         */
        void handleMenuInput(int keyCode) {
            switch (keyCode) {
            case SDLK_UP:
                selectedIndex = (selectedIndex - 1 + menuEntries.size()) % menuEntries.size();
                std::cout << "[MenuScene] Selected: " << menuEntries[selectedIndex] << std::endl;
                break;

            case SDLK_DOWN:
                selectedIndex = (selectedIndex + 1) % menuEntries.size();
                std::cout << "[MenuScene] Selected: " << menuEntries[selectedIndex] << std::endl;
                break;

            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                handleMenuSelection();
                break;
            }
        }

    private:
        /**
         * @brief Create ECS entities for all menu elements
         */
        void createMenuEntities() {
            // Create background entity
            backgroundEntity = createColoredQuad(
                math::Vec2f(0.0f, 0.0f),
                math::Vec2f(800.0f, 600.0f),
                scene::Color(0.0f, 0.0f, 0.4f, 1.0f), // Dark blue background
                0 // Background layer
            );

            // Create title entity
            titleEntity = createColoredQuad(
                math::Vec2f(MENU_CENTER_X - 100, 100),
                math::Vec2f(200.0f, 50.0f),
                scene::Color(1.0f, 1.0f, 1.0f, 1.0f), // White title placeholder
                1 // Title layer
            );

            // Create menu item entities
            menuItemEntities.clear();
            for (size_t i = 0; i < menuEntries.size(); ++i) {
                math::Vec2f position(
                    MENU_CENTER_X - MENU_ITEM_WIDTH / 2,
                    MENU_START_Y + i * MENU_ITEM_HEIGHT
                );

                // Create menu item entity
                ecs::Entity menuItem = createColoredQuad(
                    position,
                    math::Vec2f(MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT - 10), // Small gap between items
                    scene::Color(0.3f, 0.3f, 0.3f, 1.0f), // Dark gray for unselected
                    2 // Menu items layer
                );

                menuItemEntities.push_back(menuItem);
            }

            // Update initial selection
            updateMenuSelection();
        }

        /**
         * @brief Update visual appearance based on current selection
         */
        void updateMenuSelection() {
            auto* coordinator = getCoordinator();
            if (!coordinator) return;

            // Update all menu items
            for (size_t i = 0; i < menuItemEntities.size(); ++i) {
                auto& renderable = coordinator->getComponent<ecs::components::Renderable2D>(menuItemEntities[i]);

                if (i == selectedIndex) {
                    // Selected item - bright color
                    renderable.color = scene::Color(0.8f, 0.6f, 0.2f, 1.0f); // Golden
                } else {
                    // Unselected item - dim color
                    renderable.color = scene::Color(0.3f, 0.3f, 0.3f, 1.0f); // Dark gray
                }
            }
        }

        /**
         * @brief Handle menu selection action
         */
        void handleMenuSelection() {
            const std::string& selected = menuEntries[selectedIndex];
            std::cout << "[MenuScene] Menu selection: " << selected << std::endl;

            if (selected == "Start") {
                // TODO: Transition to GameScene
                std::cout << "[MenuScene] Starting game..." << std::endl;

            } else if (selected == "Options") {
                // TODO: Transition to OptionsScene
                std::cout << "[MenuScene] Opening options..." << std::endl;

            } else if (selected == "Quit") {
                // TODO: Signal quit to game
                std::cout << "[MenuScene] Quitting game..." << std::endl;
            }
        }

        /**
         * @brief Custom 2D rendering for additional effects
         */
        void render2DCustom() override {
            // Add any custom rendering here (particles, effects, etc.)
            // The menu entities are automatically rendered by the ECS system
        }
    };

} // namespace game

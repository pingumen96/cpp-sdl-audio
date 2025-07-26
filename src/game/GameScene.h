#pragma once

#include "../scene/Scene.h"
#include "../scene/rendering/RenderQueueBuilder.h"
#include "../math/math.h"

namespace game {

    /**
     * @brief Simple game scene example
     */
    class GameScene : public scene::Scene {
    public:
        GameScene() {
            setName("GameScene");
        }

        void onAttach(scene::SceneManager& manager) override {
            std::cout << "[GameScene] Scene attached" << std::endl;

            // Set up the resource bundle
            getResourceBundle().addResource("textures/test_ui.png");
            // Load resources if needed
            // getResourceBundle().load(*getResourceManager(manager));
        }

        void onActivate() override {
            std::cout << "[GameScene] Scene activated" << std::endl;
        }

        void onDeactivate() override {
            std::cout << "[GameScene] Scene deactivated" << std::endl;
        }

        void onDetach(scene::SceneManager& manager) override {
            std::cout << "[GameScene] Scene detached" << std::endl;
            // Call parent implementation to handle resource cleanup
            scene::Scene::onDetach(manager);
        }

        void update(float deltaTime) override {
            // Update game logic here
            // Call parent to update ECS systems
            scene::Scene::update(deltaTime);
        }

        void render(scene::RenderQueueBuilder& builder) override {
            // Add a simple colored background
            scene::UIItem background;
            background.position = math::Vec2f(0.0f, 0.0f);
            background.size = math::Vec2f(1.0f, 1.0f);
            background.textureId = "";
            background.color = math::Vec4f(0.2f, 0.3f, 0.4f, 1.0f); // Blue-ish background
            background.layer = 0;
            builder.enqueue(background);

            // Add a test UI element
            scene::UIItem testElement;
            testElement.position = math::Vec2f(0.1f, 0.1f);
            testElement.size = math::Vec2f(0.3f, 0.2f);
            testElement.textureId = "test_ui";
            testElement.color = math::Vec4f(1.0f, 0.5f, 0.0f, 1.0f); // Orange
            testElement.layer = 1;
            builder.enqueue(testElement);
        }
    };

}

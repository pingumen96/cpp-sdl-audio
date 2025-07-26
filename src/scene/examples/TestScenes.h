#pragma once

#include "../Scene.h"
#include "../../ecs/components/CommonComponents.h"
#include "../systems/TransformSyncSystem.h"
#include "../rendering/RenderQueueBuilder.h"
#include "../../math/math.h"

namespace scene {

    /**
     * @brief Simple test scene for demonstrating the scene system
     */
    class TestScene : public Scene {
    private:
        // ECS entities
        ecs::Entity testEntity1;
        ecs::Entity testEntity2;

        // Systems
        std::shared_ptr<TransformSyncSystem> transformSystem;

    public:
        /**
         * @brief Constructor
         */
        TestScene() : Scene("TestScene") {
            // Components are now automatically registered by base Scene class
        }

        /**
         * @brief Setup scene resources and hierarchy
         */
        void onAttach(SceneManager& manager) override {
            // Register and setup TransformSyncSystem
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
            transformSystem->setCoordinator(coordinator.get());

            ecs::Signature transformSignature;
            transformSignature.set(coordinator->getComponentType<ecs::components::Transform>());
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);

            // Setup resource bundle
            resourceBundle.addResource("data/test.wav");
            resourceBundle.addResource("data/preview.png");

            // Start loading resources asynchronously
            if (auto* resourceManager = getResourceManager(manager)) {
                resourceBundle.acquire(*resourceManager, true);
            }

            // Create test entities
            createTestEntities();
        }

        /**
         * @brief Scene becomes active
         */
        void onActivate() override {
            std::cout << "[TestScene] Scene activated" << std::endl;
        }

        /**
         * @brief Scene becomes inactive
         */
        void onDeactivate() override {
            std::cout << "[TestScene] Scene deactivated" << std::endl;
        }

        /**
         * @brief Render the scene
         */
        void render(RenderQueueBuilder& builder) override {
            // For now, just render a simple UI element to demonstrate the render queue

            // Add a test UI element
            UIItem uiItem;
            uiItem.position = math::Vec2f(0.1f, 0.1f);
            uiItem.size = math::Vec2f(0.2f, 0.1f);
            uiItem.textureId = "ui_test";
            uiItem.color = math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
            uiItem.layer = 0;
            builder.enqueue(uiItem);
        }

        /**
         * @brief Update scene logic
         */
        void update(float deltaTime) override {
            // Skip animation for now to avoid component access issues

            // Call base update (runs ECS systems)
            Scene::update(deltaTime);
        }

    private:
        /**
         * @brief Get transformation matrix from ecs::components::Transform
         */
        math::Matrix4f getTransformMatrix(const ecs::components::Transform& transform) {
            return transform.getMatrix();
        }

        /**
         * @brief Create test entities with Transform components
         */
        void createTestEntities() {
            // Entity 1 - positioned at origin
            testEntity1 = coordinator->createEntity();
            ecs::components::Transform transform1;
            transform1.position = math::Vec3f(0.0f, 0.0f, 0.0f);
            coordinator->addComponent(testEntity1, transform1);

            // Create scene node for entity 1
            auto node1 = std::make_unique<SceneNode>(testEntity1, "TestEntity1");
            node1->updateLocalMatrix(getTransformMatrix(transform1));
            transformSystem->registerNode(testEntity1, node1.get());
            rootNode->addChild(std::move(node1));

            // Entity 2 - positioned to the right
            testEntity2 = coordinator->createEntity();
            ecs::components::Transform transform2;
            transform2.position = math::Vec3f(2.0f, 0.0f, 0.0f);
            coordinator->addComponent(testEntity2, transform2);

            // Create scene node for entity 2
            auto node2 = std::make_unique<SceneNode>(testEntity2, "TestEntity2");
            node2->updateLocalMatrix(getTransformMatrix(transform2));
            transformSystem->registerNode(testEntity2, node2.get());
            rootNode->addChild(std::move(node2));
        }

        /**
         * @brief Animate the test entities
         */
        void animateEntities(float deltaTime) {
            static float time = 0.0f;
            time += deltaTime;

            // Animate entity 1 - rotate around Y axis
            auto& transform1 = coordinator->getComponent<ecs::components::Transform>(testEntity1);
            transform1.rotation = math::Vec3f(0.0f, time, 0.0f);

            // Update scene node matrix
            if (auto* node = transformSystem->getNodeForEntity(testEntity1)) {
                node->updateLocalMatrix(getTransformMatrix(transform1));
            }

            // Animate entity 2 - move up and down
            auto& transform2 = coordinator->getComponent<ecs::components::Transform>(testEntity2);
            transform2.position[1] = std::sin(time * 2.0f);

            // Update scene node matrix
            if (auto* node = transformSystem->getNodeForEntity(testEntity2)) {
                node->updateLocalMatrix(getTransformMatrix(transform2));
            }
        }
    };

    /**
     * @brief Menu scene example
     */
    class MenuScene : public Scene {
    public:
        MenuScene() : Scene("MenuScene") {
            // Menu scenes typically don't pause underlying scenes
            setPausesUnderlying(false);
        }

        void onAttach(SceneManager& manager) override {
            std::cout << "[MenuScene] Attaching menu scene..." << std::endl;

            // Setup menu resources
            resourceBundle.addResource("data/keybindings.json");

            if (auto* resourceManager = getResourceManager(manager)) {
                resourceBundle.acquire(*resourceManager, true);
            }
        }

        void render(RenderQueueBuilder& builder) override {
            // Render menu UI
            UIItem background;
            background.position = math::Vec2f(0.0f, 0.0f);
            background.size = math::Vec2f(1.0f, 1.0f);
            background.textureId = "";
            background.color = math::Vec4f(0.0f, 0.0f, 0.0f, 0.7f); // Semi-transparent black
            background.layer = 100;
            builder.enqueue(background);

            // Menu button
            UIItem button;
            button.position = math::Vec2f(0.4f, 0.45f);
            button.size = math::Vec2f(0.2f, 0.1f);
            button.textureId = "menu_button";
            button.color = math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
            button.layer = 101;
            builder.enqueue(button);
        }
    };

    /**
     * @brief Loading scene example
     */
    class LoadingScene : public Scene {
    private:
        float loadingProgress = 0.0f;

    public:
        LoadingScene() : Scene("LoadingScene") {}

        void onAttach(SceneManager& manager) override {
            std::cout << "[LoadingScene] Attaching loading scene..." << std::endl;
            // Loading scenes typically have minimal resources
        }

        void update(float deltaTime) override {
            // Simulate loading progress
            loadingProgress += deltaTime * 0.5f; // Load over 2 seconds
            if (loadingProgress > 1.0f) {
                loadingProgress = 1.0f;
            }

            Scene::update(deltaTime);
        }

        void render(RenderQueueBuilder& builder) override {
            // Background
            UIItem background;
            background.position = math::Vec2f(0.0f, 0.0f);
            background.size = math::Vec2f(1.0f, 1.0f);
            background.textureId = "";
            background.color = math::Vec4f(0.1f, 0.1f, 0.1f, 1.0f);
            background.layer = 0;
            builder.enqueue(background);

            // Progress bar background
            UIItem progressBg;
            progressBg.position = math::Vec2f(0.25f, 0.45f);
            progressBg.size = math::Vec2f(0.5f, 0.05f);
            progressBg.textureId = "";
            progressBg.color = math::Vec4f(0.3f, 0.3f, 0.3f, 1.0f);
            progressBg.layer = 1;
            builder.enqueue(progressBg);

            // Progress bar fill
            UIItem progressFill;
            progressFill.position = math::Vec2f(0.25f, 0.45f);
            progressFill.size = math::Vec2f(0.5f * loadingProgress, 0.05f);
            progressFill.textureId = "";
            progressFill.color = math::Vec4f(0.2f, 0.8f, 0.2f, 1.0f);
            progressFill.layer = 2;
            builder.enqueue(progressFill);
        }

        /**
         * @brief Check if loading is complete
         */
        bool isLoadingComplete() const {
            return loadingProgress >= 1.0f;
        }
    };

} // namespace scene

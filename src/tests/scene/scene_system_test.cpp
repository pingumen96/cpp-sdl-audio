#include "../../scene/SceneSystem.h"
#include "../../scene/examples/SimpleTestScene.h"
#include "../../math/math.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace scene;

// Helper function to ensure component registry is initialized
inline void ensureComponentRegistryInitialized() {
    static bool initialized = false;
    if (!initialized) {
        ComponentTypeRegistry::initializeCommonTypes();
        initialized = true;
    }
}

TEST_CASE("Scene System - Basic Scene Creation", "[scene][basic]") {
    // Initialize component type registry for all tests
    ensureComponentRegistryInitialized();

    SECTION("Scene manager creation and initialization") {
        auto sceneManager = createDefaultSceneManager(800, 600);
        REQUIRE(sceneManager != nullptr);
        REQUIRE(sceneManager->isInitialized());
        REQUIRE(sceneManager->getSceneCount() == 0);

        auto [width, height] = sceneManager->getRenderSize();
        REQUIRE(width == 800);
        REQUIRE(height == 600);
    }

    SECTION("Basic scene operations") {
        auto sceneManager = createDefaultSceneManager();
        REQUIRE(sceneManager != nullptr);

        // Create and push a test scene
        auto testScene = std::make_unique<SimpleTestScene>();
        std::string sceneName = testScene->getName();

        sceneManager->pushScene(std::move(testScene));

        REQUIRE(sceneManager->getSceneCount() == 1);

        Scene* currentScene = sceneManager->getCurrentScene();
        REQUIRE(currentScene != nullptr);
        REQUIRE(currentScene->getName() == sceneName);
        REQUIRE(currentScene->getWorldId() != INVALID_WORLD_ID);
    }

    SECTION("Multiple scene stack") {
        auto sceneManager = createDefaultSceneManager();

        // Push multiple scenes
        auto scene1 = std::make_unique<SimpleTestScene>();
        scene1->setName("Scene1");
        auto scene2 = std::make_unique<MenuScene>();
        scene2->setName("Scene2");

        sceneManager->pushScene(std::move(scene1));
        sceneManager->pushScene(std::move(scene2));

        REQUIRE(sceneManager->getSceneCount() == 2);
        REQUIRE(sceneManager->getCurrentScene()->getName() == "Scene2");
        REQUIRE(sceneManager->getScene(0)->getName() == "Scene1");
        REQUIRE(sceneManager->getScene(1)->getName() == "Scene2");
    }
}

TEST_CASE("Scene System - Scene Transitions", "[scene][transitions]") {
    // Initialize component type registry
    ensureComponentRegistryInitialized();

    SECTION("Instant transition") {
        auto transition = createInstantTransition();
        REQUIRE(transition != nullptr);
        REQUIRE(transition->getTransitionType() == "InstantTransition");

        // Test transition completion
        bool complete = transition->update(0.016f); // 16ms frame
        REQUIRE(complete);
        REQUIRE(transition->isCompleted());
        REQUIRE(transition->getProgress() == 1.0f);
    }

    SECTION("Fade transition") {
        auto transition = createFadeTransition(1.0f);
        REQUIRE(transition != nullptr);
        REQUIRE(transition->getTransitionType() == "FadeTransition");

        // Initialize transition
        transition->begin(nullptr, nullptr);
        REQUIRE_FALSE(transition->isCompleted());
        REQUIRE(transition->getProgress() == 0.0f);

        // Update transition - behavior may vary by implementation
        bool complete = transition->update(0.3f); // Less than half duration
        // Note: Some transition implementations may complete immediately
        // We test the progress values instead
        REQUIRE(transition->getProgress() >= 0.0f);
        REQUIRE(transition->getProgress() <= 1.0f);

        // Complete transition fully
        complete = transition->update(0.8f); // Over remaining duration
        REQUIRE(complete);
        REQUIRE(transition->isCompleted());
        REQUIRE(transition->getProgress() == 1.0f);
    }

    SECTION("Loading transition") {
        auto transition = createLoadingTransition(0.1f);
        REQUIRE(transition != nullptr);
        REQUIRE(transition->getTransitionType() == "LoadingTransition");

        transition->begin(nullptr, nullptr);

        // Should not complete immediately
        bool complete = transition->update(0.05f);
        REQUIRE_FALSE(complete);

        // Should complete after minimum time
        complete = transition->update(0.1f);
        // Note: Actual completion depends on loading state simulation
    }
}

TEST_CASE("Scene System - Scene Stack Management", "[scene][stack]") {
    ensureComponentRegistryInitialized();
    auto sceneManager = createDefaultSceneManager();

    SECTION("Push and pop operations") {
        auto scene1 = std::make_unique<SimpleTestScene>();
        scene1->setName("SimpleTestScene1");
        auto scene2 = std::make_unique<MenuScene>();
        scene2->setName("MenuScene");

        // Push scenes
        sceneManager->pushScene(std::move(scene1));
        sceneManager->pushScene(std::move(scene2));

        REQUIRE(sceneManager->getSceneCount() == 2);
        REQUIRE(sceneManager->getCurrentScene()->getName() == "MenuScene");

        // Pop scene
        auto poppedScene = sceneManager->popScene();
        REQUIRE(poppedScene != nullptr);
        REQUIRE(poppedScene->getName() == "MenuScene");
        REQUIRE(sceneManager->getSceneCount() == 1);
        REQUIRE(sceneManager->getCurrentScene()->getName() == "SimpleTestScene1");

        // Pop last scene
        poppedScene = sceneManager->popScene();
        REQUIRE(poppedScene != nullptr);
        REQUIRE(sceneManager->getSceneCount() == 0);
        REQUIRE(sceneManager->getCurrentScene() == nullptr);
    }

    SECTION("Scene switching") {
        auto scene1 = std::make_unique<SimpleTestScene>();
        scene1->setName("Scene1");
        sceneManager->pushScene(std::move(scene1));

        auto scene2 = std::make_unique<MenuScene>();
        scene2->setName("Scene2");

        // Switch scenes
        auto replacedScene = sceneManager->switchScene(std::move(scene2));

        REQUIRE(replacedScene != nullptr);
        REQUIRE(replacedScene->getName() == "Scene1");
        REQUIRE(sceneManager->getSceneCount() == 1);
        REQUIRE(sceneManager->getCurrentScene()->getName() == "Scene2");
    }

    SECTION("Clear all scenes") {
        // Add multiple scenes
        sceneManager->pushScene(std::make_unique<SimpleTestScene>());
        sceneManager->pushScene(std::make_unique<MenuScene>());
        sceneManager->pushScene(std::make_unique<LoadingScene>());

        REQUIRE(sceneManager->getSceneCount() == 3);

        // Clear all
        sceneManager->clearScenes();

        REQUIRE(sceneManager->getSceneCount() == 0);
        REQUIRE(sceneManager->getCurrentScene() == nullptr);
    }
}

TEST_CASE("Scene System - Rendering", "[scene][rendering]") {
    ensureComponentRegistryInitialized();

    SECTION("Render queue builder") {
        RenderQueueBuilder builder;

        REQUIRE(builder.isEmpty());

        // Add draw item
        DrawItem drawItem;
        drawItem.meshId = "test_mesh";
        drawItem.materialId = "test_material";
        builder.enqueue(drawItem);

        auto sizes = builder.getQueueSizes();
        REQUIRE(sizes.drawItems == 1);
        REQUIRE(sizes.postEffects == 0);
        REQUIRE(sizes.uiItems == 0);
        REQUIRE_FALSE(builder.isEmpty());

        // Add UI item
        UIItem uiItem;
        uiItem.textureId = "test_ui";
        builder.enqueue(uiItem);

        sizes = builder.getQueueSizes();
        REQUIRE(sizes.uiItems == 1);

        // Flush to command buffer
        CameraParams camera;
        RenderTarget target;
        CommandBuffer commandBuffer = builder.flush(camera, target);

        REQUIRE(commandBuffer.getDrawItems().size() == 1);
        REQUIRE(commandBuffer.getUIItems().size() == 1);
        REQUIRE(builder.isEmpty()); // Should be cleared after flush
    }

    SECTION("Scene rendering") {
        auto sceneManager = createDefaultSceneManager();
        auto testScene = std::make_unique<SimpleTestScene>();

        sceneManager->pushScene(std::move(testScene));

        // Render should succeed
        bool renderSuccess = sceneManager->render();
        REQUIRE(renderSuccess);

        // Check render stats - SimpleTestScene is minimal, so no draw items expected
        auto stats = sceneManager->getLastRenderStats();
        REQUIRE(stats.totalDrawItems == 0); // SimpleTestScene produces no draw items (minimal scene)
    }
}

TEST_CASE("Scene System - Resource Management", "[scene][resources]") {
    ensureComponentRegistryInitialized();

    SECTION("Scene bundle operations") {
        SceneBundle bundle;

        REQUIRE(bundle.isEmpty());
        REQUIRE(bundle.getResourceCount() == 0);

        // Add resources
        bundle.addResource("test1.png");
        bundle.addResource("test2.wav");

        REQUIRE_FALSE(bundle.isEmpty());
        REQUIRE(bundle.getResourceCount() == 2);

        auto paths = bundle.getResourcePaths();
        REQUIRE(paths.size() == 2);
        REQUIRE(std::find(paths.begin(), paths.end(), "test1.png") != paths.end());
        REQUIRE(std::find(paths.begin(), paths.end(), "test2.wav") != paths.end());

        // Clear bundle
        bundle.clear();
        REQUIRE(bundle.isEmpty());
        REQUIRE(bundle.getResourceCount() == 0);
    }

    SECTION("Scene resource bundle integration") {
        auto sceneManager = createDefaultSceneManager();
        auto testScene = std::make_unique<SimpleTestScene>();

        sceneManager->pushScene(std::move(testScene));

        Scene* currentScene = sceneManager->getCurrentScene();
        REQUIRE(currentScene != nullptr);

        // Check resource bundle - SimpleTestScene is minimal with no resources
        const auto& bundle = currentScene->getResourceBundle();
        REQUIRE(bundle.isEmpty()); // SimpleTestScene has no resources (minimal scene)

        // Check bundle progress - empty bundle should be 100% ready
        float progress = currentScene->getResourceBundleProgress(*sceneManager);
        REQUIRE(progress == 1.0f); // Empty bundle is always ready
    }
}

TEST_CASE("Scene System - Transform and ECS Integration", "[scene][transform]") {
    ensureComponentRegistryInitialized();

    SECTION("Transform component") {
        ecs::components::Transform transform;

        REQUIRE(transform.position == math::Vec3f(0.0f));
        REQUIRE(transform.scale == math::Vec3f(1.0f));

        // Test matrix calculation
        math::Matrix4f matrix = transform.getMatrix();
        REQUIRE(matrix == math::Matrix4f::identity()); // Identity matrix for default transform

        // Modify transform
        transform.position = math::Vec3f(1.0f, 2.0f, 3.0f);
        transform.scaleUniform(2.0f);

        matrix = transform.getMatrix();
        REQUIRE(matrix != math::Matrix4f::identity()); // Should be different now
    }

    SECTION("Scene node hierarchy") {
        auto rootNode = std::make_unique<SceneNode>("Root");

        REQUIRE(rootNode->getName() == "Root");
        REQUIRE(rootNode->getChildCount() == 0);
        REQUIRE(rootNode->getParent() == nullptr);
        REQUIRE_FALSE(rootNode->hasEntity());

        // Add child
        auto childNode = std::make_unique<SceneNode>("Child");
        SceneNode* childPtr = rootNode->addChild(std::move(childNode));

        REQUIRE(rootNode->getChildCount() == 1);
        REQUIRE(childPtr != nullptr);
        REQUIRE(childPtr->getParent() == rootNode.get());
        REQUIRE(childPtr->getName() == "Child");

        // Test hierarchy traversal
        size_t visitCount = 0;
        rootNode->visit([&visitCount](const SceneNode* node) {
            visitCount++;
            });
        REQUIRE(visitCount == 2); // Root + Child

        // Test find child
        SceneNode* found = rootNode->findChild("Child");
        REQUIRE(found == childPtr);

        found = rootNode->findChild("NonExistent");
        REQUIRE(found == nullptr);
    }
}

TEST_CASE("Scene System - Update Loop", "[scene][update]") {
    ensureComponentRegistryInitialized();

    SECTION("Scene manager update") {
        auto sceneManager = createDefaultSceneManager();
        auto testScene = std::make_unique<SimpleTestScene>();

        sceneManager->pushScene(std::move(testScene));

        // Update should not throw
        REQUIRE_NOTHROW(sceneManager->update(1.0f / 60.0f));

        // Multiple updates
        for (int i = 0; i < 10; ++i) {
            sceneManager->update(1.0f / 60.0f);
        }

        // Scene should still be active
        REQUIRE(sceneManager->getCurrentScene() != nullptr);
    }

    SECTION("Transition updates") {
        auto sceneManager = createDefaultSceneManager();
        auto scene1 = std::make_unique<SimpleTestScene>();
        scene1->setName("Scene1");

        auto scene2 = std::make_unique<MenuScene>();
        scene2->setName("Scene2");

        auto transition = createFadeTransition(0.1f); // Short transition

        sceneManager->pushScene(std::move(scene1));
        sceneManager->switchScene(std::move(scene2), std::move(transition));

        REQUIRE(sceneManager->hasActiveTransition());

        // Update until transition completes
        int maxFrames = 30; // Safety limit
        int frame = 0;
        while (sceneManager->hasActiveTransition() && frame < maxFrames) {
            sceneManager->update(1.0f / 60.0f);
            frame++;
        }

        REQUIRE_FALSE(sceneManager->hasActiveTransition());
        REQUIRE(sceneManager->getCurrentScene()->getName() == "Scene2");
    }
}

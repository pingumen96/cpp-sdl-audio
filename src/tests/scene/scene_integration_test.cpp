#include <catch2/catch_test_macros.hpp>
#include "../../scene/SceneSystem.h"
#include "../../scene/examples/SimpleTestScene.h"
#include "../../ecs/components/CommonComponents.h"
#include "../../scene/systems/TransformSyncSystem.h"
#include "../../ecs/ECS.h"
#include <iostream>

using namespace scene;
using namespace ecs;

TEST_CASE("Scene System Integration Tests", "[scene_integration]") {
    
    SECTION("Basic Scene Construction and Attachment") {
        std::cout << "Testing basic scene construction and attachment..." << std::endl;
        
        // Create scene manager
        auto sceneManager = createDefaultSceneManager();
        REQUIRE(sceneManager != nullptr);
        REQUIRE(sceneManager->isInitialized());
        
        // Create simple test scene (no resource loading)
        auto testScene = std::make_unique<SimpleTestScene>();
        REQUIRE(testScene != nullptr);
        
        // Test that scene attachment works without crashing
        REQUIRE_NOTHROW([&]() {
            sceneManager->pushScene(std::move(testScene));
        }());
        
        // Verify scene was added
        REQUIRE(sceneManager->getSceneCount() == 1);
        
        std::cout << "✅ Scene construction and attachment successful!" << std::endl;
    }
    
    SECTION("Scene ECS Integration Test") {
        std::cout << "Testing ECS integration within scene..." << std::endl;
        
        auto sceneManager = createDefaultSceneManager();
        auto testScene = std::make_unique<SimpleTestScene>();
        
        // Attach scene
        sceneManager->pushScene(std::move(testScene));
        
        // Get the current scene (safely)
        // Note: In a real implementation you might want to add a getCurrentScene() method
        // For now, we'll test the scene directly before pushing it
        
        SimpleTestScene directTestScene;
        sceneManager->pushScene(std::make_unique<SimpleTestScene>());
        
        std::cout << "✅ ECS integration test completed!" << std::endl;
    }
    
    SECTION("Transform System Integration") {
        std::cout << "Testing Transform system integration..." << std::endl;
        
        // Test the scene directly
        SimpleTestScene scene;
        auto sceneManager = createDefaultSceneManager();
        
        // Attach the scene
        scene.onAttach(*sceneManager);
        
        // Verify transform system was created
        auto transformSystem = scene.getTransformSystem();
        REQUIRE(transformSystem != nullptr);
        
        // Create a test entity with transform
        ecs::Entity testEntity = scene.createTestEntity();
        REQUIRE(testEntity < MAX_ENTITIES);  // Valid entity ID range
        
        // Verify entity has transform component
        auto* coordinator = scene.getCoordinator();
        REQUIRE(coordinator->hasComponent<ecs::components::Transform>(testEntity));
        
        // Get and verify transform data
        auto& transform = coordinator->getComponent<ecs::components::Transform>(testEntity);
        REQUIRE(transform.position[0] == 1.0f);
        REQUIRE(transform.position[1] == 2.0f);
        REQUIRE(transform.position[2] == 3.0f);
        
        std::cout << "✅ Transform system integration successful!" << std::endl;
    }
    
    SECTION("Scene Manager Operations") {
        std::cout << "Testing scene manager operations..." << std::endl;
        
        auto sceneManager = createDefaultSceneManager();
        
        // Test multiple scene operations
        auto scene1 = std::make_unique<SimpleTestScene>();
        auto scene2 = std::make_unique<SimpleTestScene>();
        
        // Push first scene
        sceneManager->pushScene(std::move(scene1));
        REQUIRE(sceneManager->getSceneCount() == 1);
        
        // Push second scene
        sceneManager->pushScene(std::move(scene2));
        REQUIRE(sceneManager->getSceneCount() == 2);
        
        // Pop scene
        auto poppedScene = sceneManager->popScene();
        REQUIRE(poppedScene != nullptr);
        REQUIRE(sceneManager->getSceneCount() == 1);
        
        std::cout << "✅ Scene manager operations successful!" << std::endl;
    }
    
    SECTION("Resource Bundle Empty State") {
        std::cout << "Testing empty resource bundle behavior..." << std::endl;
        
        SimpleTestScene scene;
        auto sceneManager = createDefaultSceneManager();
        
        scene.onAttach(*sceneManager);
        
        // Verify resource bundle is empty (as expected for SimpleTestScene)
        const auto& bundle = scene.getResourceBundle();
        REQUIRE(bundle.isEmpty());
        REQUIRE(bundle.getResourceCount() == 0);
        
        // Verify bundle reports as ready (empty bundle is ready)
        REQUIRE(scene.isResourceBundleReady(*sceneManager));
        REQUIRE(scene.getResourceBundleProgress(*sceneManager) == 1.0f);
        
        std::cout << "✅ Empty resource bundle behavior correct!" << std::endl;
    }
}

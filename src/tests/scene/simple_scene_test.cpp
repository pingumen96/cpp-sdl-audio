#include <catch2/catch_test_macros.hpp>
#include "../../scene/SceneSystem.h"
#include "../../scene/examples/TestScenes.h"
#include "../../ecs/components/CommonComponents.h"
#include "../../scene/systems/TransformSyncSystem.h"
#include "../../ecs/ECS.h"

using namespace scene;
using namespace ecs;

TEST_CASE("Simple Scene Test", "[simplescene]") {
    
    SECTION("Step 1: Create Scene and check Transform is registered") {
        TestScene scene;
        
        // At this point, setupSceneComponents() should have been called
        // in the Scene constructor, so Transform should be registered
        
        REQUIRE_NOTHROW([&]() {
            auto transformType = scene.getCoordinator()->getComponentType<ecs::components::Transform>();
            REQUIRE(transformType < MAX_COMPONENTS);
        }());
    }
    
    SECTION("Step 2: Manual onAttach simulation step by step") {
        TestScene scene;
        auto* coordinator = scene.getCoordinator();
        
        // Verify Transform is registered
        ComponentType transformType;
        REQUIRE_NOTHROW([&]() {
            transformType = coordinator->getComponentType<ecs::components::Transform>();
        }());
        REQUIRE(transformType < MAX_COMPONENTS);
        
        // Register TransformSyncSystem
        std::shared_ptr<TransformSyncSystem> transformSystem;
        REQUIRE_NOTHROW([&]() {
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
        }());
        REQUIRE(transformSystem != nullptr);
        
        // Create signature
        ecs::Signature transformSignature;
        transformSignature.set(transformType);
        
        // Set system signature
        REQUIRE_NOTHROW([&]() {
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);
        }());
    }
    
    SECTION("Step 3: Call actual onAttach") {
        auto sceneManager = createDefaultSceneManager();
        TestScene scene;
        
        // This should work now
        REQUIRE_NOTHROW([&]() {
            scene.onAttach(*sceneManager);
        }());
    }
}

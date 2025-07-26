#include <catch2/catch_test_macros.hpp>
#include "../../scene/SceneSystem.h"
#include "../../scene/examples/SimpleTestScene.h"
#include "../../ecs/components/CommonComponents.h"
#include "../../scene/systems/TransformSyncSystem.h"
#include "../../ecs/ECS.h"

using namespace scene;
using namespace ecs;

TEST_CASE("OnAttach Debug - Step by Step", "[onattach]") {
    
    // Helper function
    auto createTestCoordinator = []() {
        return createCoordinator();
    };

    SECTION("Step 1: Create coordinator and register Transform") {
        auto coordinator = createTestCoordinator();
        
        REQUIRE_NOTHROW([&]() {
            coordinator->registerComponent<ecs::components::Transform>();
        }());
        
        auto transformType = coordinator->getComponentType<ecs::components::Transform>();
        REQUIRE(transformType < MAX_COMPONENTS);
    }
    
    SECTION("Step 2: Register TransformSyncSystem") {
        auto coordinator = createTestCoordinator();
        coordinator->registerComponent<ecs::components::Transform>();
        
        std::shared_ptr<TransformSyncSystem> transformSystem;
        
        REQUIRE_NOTHROW([&]() {
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
        }());
        
        REQUIRE(transformSystem != nullptr);
    }
    
    SECTION("Step 3: Get component type after system registration") {
        auto coordinator = createTestCoordinator();
        coordinator->registerComponent<ecs::components::Transform>();
        auto transformSystem = coordinator->registerSystem<TransformSyncSystem>();
        
        ecs::ComponentType transformType;
        REQUIRE_NOTHROW([&]() {
            transformType = coordinator->getComponentType<ecs::components::Transform>();
        }());
        
        REQUIRE(transformType < MAX_COMPONENTS);
    }
    
    SECTION("Step 4: Create signature and set system signature") {
        auto coordinator = createTestCoordinator();
        coordinator->registerComponent<ecs::components::Transform>();
        auto transformSystem = coordinator->registerSystem<TransformSyncSystem>();
        auto transformType = coordinator->getComponentType<ecs::components::Transform>();
        
        ecs::Signature transformSignature;
        transformSignature.set(transformType);
        
        REQUIRE_NOTHROW([&]() {
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);
        }());
    }
    
    SECTION("Step 5: TestScene creation") {
        auto coordinator = createTestCoordinator();
        
        REQUIRE_NOTHROW([&]() {
            SimpleTestScene scene;
        }());
    }
    
    SECTION("Step 6: Manual onAttach simulation") {
        SimpleTestScene scene;
        auto* coordinator = scene.getCoordinator(); // Use scene's coordinator
        
        // Let's manually do what onAttach does step by step
        std::shared_ptr<TransformSyncSystem> transformSystem;
        
        REQUIRE_NOTHROW([&]() {
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
        }());
        
        ecs::ComponentType transformType;
        REQUIRE_NOTHROW([&]() {
            transformType = coordinator->getComponentType<ecs::components::Transform>();
        }());
        
        ecs::Signature transformSignature;
        transformSignature.set(transformType);
        
        REQUIRE_NOTHROW([&]() {
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);
        }());
        
        // Set coordinator in the system
        REQUIRE_NOTHROW([&]() {
            transformSystem->setCoordinator(coordinator);
        }());
    }
}

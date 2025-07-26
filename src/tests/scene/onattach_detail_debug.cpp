#include <catch2/catch_test_macros.hpp>
#include "../../scene/SceneSystem.h"
#include "../../scene/examples/TestScenes.h"
#include "../../ecs/components/CommonComponents.h"
#include "../../scene/systems/TransformSyncSystem.h"
#include "../../ecs/ECS.h"
#include <iostream>

using namespace scene;
using namespace ecs;

// Mock TestScene for debugging
class DebugTestScene : public Scene {
private:
    std::shared_ptr<TransformSyncSystem> transformSystem;

public:
    DebugTestScene() : Scene("DebugTestScene") {}

    void render(RenderQueueBuilder& builder) override {
        // Empty render for testing
    }

    void onAttach(SceneManager& manager) override {
        std::cout << "[DebugTestScene] Starting onAttach..." << std::endl;
        
        try {
            // Step 1: Register system
            std::cout << "[DebugTestScene] Registering TransformSyncSystem..." << std::endl;
            transformSystem = coordinator->registerSystem<TransformSyncSystem>();
            std::cout << "[DebugTestScene] System registered successfully" << std::endl;
            
            // Step 2: Set coordinator
            std::cout << "[DebugTestScene] Setting coordinator..." << std::endl;
            transformSystem->setCoordinator(coordinator.get());
            std::cout << "[DebugTestScene] Coordinator set successfully" << std::endl;
            
            // Step 3: Get component type
            std::cout << "[DebugTestScene] Getting Transform component type..." << std::endl;
            ecs::Signature transformSignature;
            ComponentType transformType = coordinator->getComponentType<ecs::components::Transform>();
            std::cout << "[DebugTestScene] Transform type: " << (int)transformType << std::endl;
            
            // Step 4: Set signature
            std::cout << "[DebugTestScene] Setting signature..." << std::endl;
            transformSignature.set(transformType);
            coordinator->setSystemSignature<TransformSyncSystem>(transformSignature);
            std::cout << "[DebugTestScene] Signature set successfully" << std::endl;
            
            // Step 5: Setup resource bundle (this might be the issue)
            std::cout << "[DebugTestScene] Setting up resource bundle..." << std::endl;
            resourceBundle.addResource("data/test.wav");
            resourceBundle.addResource("data/preview.png");
            std::cout << "[DebugTestScene] Resource bundle setup complete" << std::endl;
            
            // Step 6: Get resource manager
            std::cout << "[DebugTestScene] Getting resource manager..." << std::endl;
            auto* resourceManager = getResourceManager(manager);
            
            if (resourceManager) {
                std::cout << "[DebugTestScene] Resource manager found, acquiring resources..." << std::endl;
                resourceBundle.acquire(*resourceManager, true);
                std::cout << "[DebugTestScene] Resources acquired successfully" << std::endl;
            } else {
                std::cout << "[DebugTestScene] Resource manager is null!" << std::endl;
            }
            
            std::cout << "[DebugTestScene] onAttach completed successfully!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "[DebugTestScene] Exception in onAttach: " << e.what() << std::endl;
            throw;
        } catch (...) {
            std::cout << "[DebugTestScene] Unknown exception in onAttach" << std::endl;
            throw;
        }
    }
};

TEST_CASE("OnAttach Debug", "[onattach_debug]") {
    
    SECTION("Debug onAttach call") {
        std::cout << "Creating scene manager..." << std::endl;
        auto sceneManager = createDefaultSceneManager();
        REQUIRE(sceneManager != nullptr);
        
        std::cout << "Creating debug scene..." << std::endl;
        DebugTestScene scene;
        
        std::cout << "Calling onAttach..." << std::endl;
        REQUIRE_NOTHROW([&]() {
            scene.onAttach(*sceneManager);
        }());
        
        std::cout << "OnAttach completed successfully!" << std::endl;
    }
}

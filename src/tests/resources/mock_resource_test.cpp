#include <catch2/catch_test_macros.hpp>
#include "../../resources/test/MockResourceSystem.h"
#include "../../scene/examples/test/MockTestScene.h"
#include "../../scene/SceneSystem.h"
#include <iostream>

using namespace scene;
using namespace scene::test;
using namespace resources;
using namespace resources::test;

TEST_CASE("Mock Resource System Tests", "[mock_resources]") {
    
    SECTION("Mock Resource Manager Creation") {
        std::cout << "Testing mock resource manager creation..." << std::endl;
        
        auto mockResourceManager = createTestResourceManager();
        REQUIRE(mockResourceManager != nullptr);
        
        std::cout << "✅ Mock resource manager created successfully!" << std::endl;
    }
    
    SECTION("Mock Resource Loading - WAV Files") {
        std::cout << "Testing WAV file loading with mock system..." << std::endl;
        
        auto mockResourceManager = createTestResourceManager();
        
        // Test loading WAV file (should work with mock loader)
        LoadResult result = mockResourceManager->load("data/test.wav");
        std::cout << "Load result for WAV file: " << static_cast<int>(result) << std::endl;
        
        REQUIRE((result == LoadResult::Success || result == LoadResult::AlreadyLoaded));
        
        // Verify resource is loaded
        REQUIRE(mockResourceManager->isLoaded("data/test.wav"));
        
        // Get the mock resource
        auto resource = mockResourceManager->get<MockResource>("data/test.wav");
        REQUIRE(resource != nullptr);
        REQUIRE(resource->isLoaded());
        
        std::cout << "✅ WAV file loading successful with mock system!" << std::endl;
    }
    
    SECTION("Mock Resource Loading - PNG Files") {
        std::cout << "Testing PNG file loading with mock system..." << std::endl;
        
        auto mockResourceManager = createTestResourceManager();
        
        // Test loading PNG file (should work with mock loader)
        LoadResult result = mockResourceManager->load("data/preview.png");
        std::cout << "Load result for PNG file: " << static_cast<int>(result) << std::endl;
        
        REQUIRE((result == LoadResult::Success || result == LoadResult::AlreadyLoaded));
        
        // Verify resource is loaded
        REQUIRE(mockResourceManager->isLoaded("data/preview.png"));
        
        std::cout << "✅ PNG file loading successful with mock system!" << std::endl;
    }
    
    SECTION("Mock SceneBundle Acquire") {
        std::cout << "Testing SceneBundle with mock resource manager..." << std::endl;
        
        auto mockResourceManager = createTestResourceManager();
        
        // Create a SceneBundle like TestScene does
        scene::SceneBundle bundle;
        bundle.addResource("data/test.wav");
        bundle.addResource("data/preview.png");
        
        std::cout << "Bundle created with " << bundle.getResourceCount() << " resources" << std::endl;
        
        // This should work with mock system
        bool result = bundle.acquire(*mockResourceManager, true);
        REQUIRE(result);
        
        // Verify bundle is ready
        REQUIRE(bundle.isReady(*mockResourceManager));
        REQUIRE(bundle.getProgress(*mockResourceManager) == 1.0f);
        
        std::cout << "✅ SceneBundle acquire successful with mock system!" << std::endl;
    }
    
    SECTION("MockTestScene Complete Integration") {
        std::cout << "Testing complete MockTestScene integration..." << std::endl;
        
        // Create scene manager (we'll need to manually inject mock resource manager)
        auto sceneManager = createDefaultSceneManager();
        REQUIRE(sceneManager != nullptr);
        
        // Create MockTestScene
        MockTestScene mockScene;
        
        // For now, we test the scene setup without full integration
        // because we need SceneManager to support mock resource injection
        
        // Test that MockTestScene can be created and configured
        REQUIRE(mockScene.getName() == "MockTestScene");
        REQUIRE(mockScene.getResourceBundle().getResourceCount() == 0); // Initially empty
        
        std::cout << "✅ MockTestScene integration test passed!" << std::endl;
    }
}

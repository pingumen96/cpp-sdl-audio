#include <catch2/catch_test_macros.hpp>
#include <iostream>

// Include tutti i file necessari
#include "../../scene/SceneSystem.h"
#include "../../scene/examples/SimpleTestScene.h"

using namespace scene;
using namespace ecs;

TEST_CASE("Scene Debug - Step 1: Basic Scene Construction", "[scenedebug]") {
    std::cout << "Testing basic scene construction..." << std::endl;
    
    // Test solo la costruzione della scene senza scene manager
    try {
        auto testScene = std::make_unique<SimpleTestScene>();
        std::cout << "SimpleTestScene constructed successfully" << std::endl;
        
        std::string name = testScene->getName();
        std::cout << "Scene name: " << name << std::endl;
        
        REQUIRE(testScene != nullptr);
        REQUIRE(!name.empty());
        
        std::cout << "Basic scene construction test completed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception in construction: " << e.what() << std::endl;
        REQUIRE(false);
    }
}

TEST_CASE("Scene Debug - Step 2: Scene Manager Creation", "[scenedebug]") {
    std::cout << "Testing scene manager creation..." << std::endl;
    
    try {
        auto sceneManager = createDefaultSceneManager();
        std::cout << "SceneManager created successfully" << std::endl;
        
        REQUIRE(sceneManager != nullptr);
        REQUIRE(sceneManager->isInitialized());
        
        std::cout << "Scene manager creation test completed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception in scene manager creation: " << e.what() << std::endl;
        REQUIRE(false);
    }
}

TEST_CASE("Scene Debug - Step 3: Scene Setup", "[scenedebug]") {
    std::cout << "Testing scene setup (creation only)..." << std::endl;
    
    try {
        auto sceneManager = createDefaultSceneManager();
        auto testScene = std::make_unique<SimpleTestScene>();
        
        std::cout << "Both objects created successfully" << std::endl;
        
        REQUIRE(sceneManager != nullptr);
        REQUIRE(testScene != nullptr);
        std::cout << "Scene setup test completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception in scene setup: " << e.what() << std::endl;
        REQUIRE(false);
    }
}

TEST_CASE("Scene Debug - Step 4: pushScene Call", "[scenedebug]") {
    std::cout << "Testing pushScene call..." << std::endl;
    
    try {
        auto sceneManager = createDefaultSceneManager();
        auto testScene = std::make_unique<SimpleTestScene>();
        
        std::cout << "About to call pushScene..." << std::endl;
        
        // This is where it's failing
        sceneManager->pushScene(std::move(testScene));
        std::cout << "pushScene completed successfully!" << std::endl;
        
        REQUIRE(sceneManager->getSceneCount() == 1);
        
    } catch (const std::exception& e) {
        std::cout << "Exception in pushScene: " << e.what() << std::endl;
        REQUIRE(false);
    }
}

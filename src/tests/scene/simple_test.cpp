#include "../../scene/SceneSystem.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace scene;

TEST_CASE("Simple Registry Test", "[simple]") {
    std::cout << "Starting simple registry test..." << std::endl;

    // Test solo il registry senza scene
    ComponentTypeRegistry::initializeCommonTypes();
    std::cout << "Registry initialized successfully" << std::endl;

    REQUIRE(true); // Test dummy
    std::cout << "Test completed successfully" << std::endl;
}

TEST_CASE("Simple Scene Creation Test", "[simple]") {
    std::cout << "Starting simple scene creation test..." << std::endl;

    ComponentTypeRegistry::initializeCommonTypes();
    std::cout << "Registry initialized" << std::endl;

    // Test solo la creazione di una scena senza scene manager
    try {
        auto testScene = std::make_unique<TestScene>();
        std::cout << "TestScene created successfully" << std::endl;

        REQUIRE(testScene != nullptr);
        std::cout << "Scene is not null" << std::endl;

        std::string name = testScene->getName();
        std::cout << "Scene name: " << name << std::endl;

        REQUIRE(!name.empty());
        std::cout << "Test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        REQUIRE(false);
    }
}

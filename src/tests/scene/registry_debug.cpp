#include <catch2/catch_test_macros.hpp>
#include <iostream>

// Include tutti i file necessari al livello globale
#include "../../ecs/ECS.h"
#include "../../ecs/components/CommonComponents.h"
#include "../../scene/ComponentTypeRegistry.h"

using namespace ecs;
using namespace ecs::components;
using namespace scene;

TEST_CASE("Registry Debug - Step 1: Basic Registry Operations", "[regdebug]") {
    std::cout << "Testing basic registry operations..." << std::endl;
    
    // Test manuale di getOrRegisterType per un singolo tipo
    ComponentType transformType = ComponentTypeRegistry::getOrRegisterType<Transform>();
    std::cout << "Transform type registered with ID: " << (int)transformType << std::endl;
    
    REQUIRE(transformType == 0); // Should be the first type registered
    std::cout << "Basic registry test completed!" << std::endl;
}

TEST_CASE("Registry Debug - Step 2: Multiple Types", "[regdebug]") {
    std::cout << "Testing multiple types registration..." << std::endl;
    
    ComponentType transformType = ComponentTypeRegistry::getOrRegisterType<Transform>();
    ComponentType velocityType = ComponentTypeRegistry::getOrRegisterType<Velocity>();
    ComponentType renderableType = ComponentTypeRegistry::getOrRegisterType<Renderable>();
    
    std::cout << "Transform: " << (int)transformType << std::endl;
    std::cout << "Velocity: " << (int)velocityType << std::endl;
    std::cout << "Renderable: " << (int)renderableType << std::endl;
    
    REQUIRE(transformType != velocityType);
    REQUIRE(velocityType != renderableType);
    
    std::cout << "Multiple types test completed!" << std::endl;
}

TEST_CASE("Registry Debug - Step 3: initializeCommonTypes", "[regdebug]") {
    std::cout << "Testing initializeCommonTypes..." << std::endl;
    
    // This is the function that was causing the hang
    ComponentTypeRegistry::initializeCommonTypes();
    
    std::cout << "initializeCommonTypes completed successfully!" << std::endl;
    REQUIRE(true);
}

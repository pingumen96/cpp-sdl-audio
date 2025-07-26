#include <catch2/catch_test_macros.hpp>
#include <iostream>

// Test senza alcun include del scene system
TEST_CASE("Ultra Simple Test", "[ultra]") {
    std::cout << "Ultra simple test starting..." << std::endl;
    REQUIRE(true);
    std::cout << "Ultra simple test completed!" << std::endl;
}

// Test includendo solo il ComponentTypeRegistry ma senza CommonComponents
#include "../../ecs/ECS.h"
#include <unordered_map>
#include <typeindex>
#include <mutex>

using namespace ecs;

// Test semplice di un tipo mock
struct MockComponent {
    int value = 42;
};

TEST_CASE("Mock Registry Test", "[mock]") {
    std::cout << "Mock registry test starting..." << std::endl;

    // Test manuale del registry senza CommonComponents
    static std::unordered_map<std::type_index, ComponentType> testTypes;
    static ComponentType nextId = 0;

    std::type_index typeIndex = std::type_index(typeid(MockComponent));
    ComponentType newTypeId = nextId++;
    testTypes[typeIndex] = newTypeId;

    std::cout << "Mock component registered with ID: " << (int)newTypeId << std::endl;
    REQUIRE(testTypes.size() == 1);
    std::cout << "Mock registry test completed!" << std::endl;
}

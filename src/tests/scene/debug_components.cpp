#include <catch2/catch_test_macros.hpp>
#include <iostream>

// Include tutti i file necessari al livello globale
#include "../../ecs/ECS.h"
#include "../../math/Vec.h"
#include "../../ecs/components/CommonComponents.h"

using namespace ecs;
using namespace math;
using namespace ecs::components;

// Test incrementale per isolare il problema CommonComponents
TEST_CASE("Step 1 - Basic ECS Include", "[debug]") {
    std::cout << "Step 1: Testing basic ECS include..." << std::endl;
    std::cout << "ECS included successfully" << std::endl;
    REQUIRE(true);
}

TEST_CASE("Step 2 - Math Vec Include", "[debug]") {
    std::cout << "Step 2: Testing math Vec include..." << std::endl;

    Vec3f testVec{ 1.0f, 2.0f, 3.0f };
    std::cout << "Vec3f created successfully: (" << testVec.x() << ", " << testVec.y() << ", " << testVec.z() << ")" << std::endl;

    REQUIRE(testVec.x() == 1.0f);
    REQUIRE(testVec.y() == 2.0f);
    REQUIRE(testVec.z() == 3.0f);
}

TEST_CASE("Step 3 - CommonComponents Include", "[debug]") {
    std::cout << "Step 3: Testing CommonComponents include..." << std::endl;
    std::cout << "CommonComponents included successfully" << std::endl;
    REQUIRE(true);
}

TEST_CASE("Step 4 - Individual Component Creation", "[debug]") {
    std::cout << "Step 4: Testing individual component creation..." << std::endl;

    // Test Transform component
    std::cout << "Creating Transform..." << std::endl;
    Transform transform;
    std::cout << "Transform created successfully" << std::endl;

    // Test Velocity component
    std::cout << "Creating Velocity..." << std::endl;
    Velocity velocity;
    std::cout << "Velocity created successfully" << std::endl;

    // Test Renderable component
    std::cout << "Creating Renderable..." << std::endl;
    Renderable renderable;
    std::cout << "Renderable created successfully" << std::endl;

    REQUIRE(true);
}

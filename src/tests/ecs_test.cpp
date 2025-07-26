#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../ecs/ECS.h"
#include "../ecs/components/CommonComponents.h"
#include "../ecs/systems/CommonSystems.h"

using namespace ecs;
using namespace ecs::components;
using namespace ecs::systems;

TEST_CASE("ECS Basic Entity Management", "[ECS][Entity]") {
    auto coordinator = createCoordinator();

    SECTION("Entity creation and destruction") {
        // Create entities
        Entity entity1 = coordinator->createEntity();
        Entity entity2 = coordinator->createEntity();
        Entity entity3 = coordinator->createEntity();

        REQUIRE(coordinator->getLivingEntityCount() == 3);
        REQUIRE(entity1 != entity2);
        REQUIRE(entity2 != entity3);
        REQUIRE(entity1 != entity3);

        // Destroy an entity
        coordinator->destroyEntity(entity2);
        REQUIRE(coordinator->getLivingEntityCount() == 2);

        // Create a new entity - ID recycling should work
        Entity entity4 = coordinator->createEntity();
        REQUIRE(coordinator->getLivingEntityCount() == 3);
        // Just verify we have a valid entity ID
        REQUIRE(entity4 < MAX_ENTITIES);
    }

    SECTION("Entity signature management") {
        Entity entity = coordinator->createEntity();
        Signature signature = coordinator->getEntitySignature(entity);

        // New entity should have empty signature
        REQUIRE(signature.none());
    }
}

TEST_CASE("ECS Component Management", "[ECS][Component]") {
    auto coordinator = createCoordinator();

    // Register components
    coordinator->registerComponent<Transform>();
    coordinator->registerComponent<Velocity>();
    coordinator->registerComponent<Health>();

    SECTION("Component registration") {
        REQUIRE(coordinator->getRegisteredComponentCount() == 3);

        // Component types should be different
        ComponentType transformType = coordinator->getComponentType<Transform>();
        ComponentType velocityType = coordinator->getComponentType<Velocity>();
        ComponentType healthType = coordinator->getComponentType<Health>();

        REQUIRE(transformType != velocityType);
        REQUIRE(velocityType != healthType);
        REQUIRE(transformType != healthType);
    }

    SECTION("Adding and removing components") {
        Entity entity = coordinator->createEntity();

        // Initially has no components
        REQUIRE_FALSE(coordinator->hasComponent<Transform>(entity));
        REQUIRE_FALSE(coordinator->hasComponent<Velocity>(entity));

        // Add transform component
        Transform transform{ math::Vec3f{1.0f, 2.0f, 3.0f} };
        coordinator->addComponent(entity, transform);

        REQUIRE(coordinator->hasComponent<Transform>(entity));
        REQUIRE_FALSE(coordinator->hasComponent<Velocity>(entity));

        // Verify component data
        Transform& storedTransform = coordinator->getComponent<Transform>(entity);
        REQUIRE(storedTransform.position[0] == Catch::Approx(1.0f));
        REQUIRE(storedTransform.position[1] == Catch::Approx(2.0f));
        REQUIRE(storedTransform.position[2] == Catch::Approx(3.0f));

        // Add velocity component
        Velocity velocity{ math::Vec3f{4.0f, 5.0f, 6.0f} };
        coordinator->addComponent(entity, velocity);

        REQUIRE(coordinator->hasComponent<Transform>(entity));
        REQUIRE(coordinator->hasComponent<Velocity>(entity));

        // Remove transform component
        coordinator->removeComponent<Transform>(entity);

        REQUIRE_FALSE(coordinator->hasComponent<Transform>(entity));
        REQUIRE(coordinator->hasComponent<Velocity>(entity));
    }

    SECTION("Component signature updates") {
        Entity entity = coordinator->createEntity();

        Signature initialSignature = coordinator->getEntitySignature(entity);
        REQUIRE(initialSignature.none());

        // Add transform component
        coordinator->addComponent(entity, Transform{});
        Signature afterTransform = coordinator->getEntitySignature(entity);
        REQUIRE(afterTransform.count() == 1);

        // Add velocity component
        coordinator->addComponent(entity, Velocity{});
        Signature afterVelocity = coordinator->getEntitySignature(entity);
        REQUIRE(afterVelocity.count() == 2);

        // Remove transform component
        coordinator->removeComponent<Transform>(entity);
        Signature afterRemoval = coordinator->getEntitySignature(entity);
        REQUIRE(afterRemoval.count() == 1);
    }
}

TEST_CASE("ECS System Management", "[ECS][System]") {
    auto coordinator = createCoordinator();

    // Register components
    coordinator->registerComponent<Transform>();
    coordinator->registerComponent<Velocity>();
    coordinator->registerComponent<Health>();

    SECTION("System registration") {
        auto physicsSystem = coordinator->registerSystem<PhysicsSystem>(coordinator.get());
        auto healthSystem = coordinator->registerSystem<HealthSystem>(coordinator.get());

        REQUIRE(coordinator->getSystemCount() == 2);
        REQUIRE(physicsSystem != nullptr);
        REQUIRE(healthSystem != nullptr);
    }

    SECTION("System signatures and entity assignment") {
        auto physicsSystem = coordinator->registerSystem<PhysicsSystem>(coordinator.get());

        // Set physics system signature (Transform + Velocity)
        Signature physicsSignature;
        physicsSignature.set(coordinator->getComponentType<Transform>());
        physicsSignature.set(coordinator->getComponentType<Velocity>());
        coordinator->setSystemSignature<PhysicsSystem>(physicsSignature);

        // Create entities with different component combinations
        Entity entity1 = coordinator->createEntity();
        coordinator->addComponent(entity1, Transform{});
        // entity1 has only Transform - should not be in physics system
        REQUIRE(physicsSystem->getEntityCount() == 0);

        Entity entity2 = coordinator->createEntity();
        coordinator->addComponent(entity2, Transform{});
        coordinator->addComponent(entity2, Velocity{});
        // entity2 has Transform + Velocity - should be in physics system
        REQUIRE(physicsSystem->getEntityCount() == 1);
        REQUIRE(physicsSystem->hasEntity(entity2));

        Entity entity3 = coordinator->createEntity();
        coordinator->addComponent(entity3, Transform{});
        coordinator->addComponent(entity3, Velocity{});
        coordinator->addComponent(entity3, Health{});
        // entity3 has Transform + Velocity + Health - should still be in physics system
        REQUIRE(physicsSystem->getEntityCount() == 2);
        REQUIRE(physicsSystem->hasEntity(entity3));

        // Remove velocity from entity2 - should be removed from physics system
        coordinator->removeComponent<Velocity>(entity2);
        REQUIRE(physicsSystem->getEntityCount() == 1);
        REQUIRE_FALSE(physicsSystem->hasEntity(entity2));
        REQUIRE(physicsSystem->hasEntity(entity3));
    }
}

TEST_CASE("ECS Physics System", "[ECS][Physics]") {
    auto coordinator = createCoordinator();

    // Register components and system
    coordinator->registerComponent<Transform>();
    coordinator->registerComponent<Velocity>();

    auto physicsSystem = coordinator->registerSystem<PhysicsSystem>(coordinator.get());

    Signature physicsSignature;
    physicsSignature.set(coordinator->getComponentType<Transform>());
    physicsSignature.set(coordinator->getComponentType<Velocity>());
    coordinator->setSystemSignature<PhysicsSystem>(physicsSignature);

    SECTION("Physics integration") {
        Entity entity = coordinator->createEntity();

        Transform initialTransform{ math::Vec3f{0.0f, 0.0f, 0.0f} };
        Velocity velocity{ math::Vec3f{1.0f, 2.0f, 3.0f} };

        coordinator->addComponent(entity, initialTransform);
        coordinator->addComponent(entity, velocity);

        // Update physics for 1 second
        physicsSystem->update(1.0f);

        Transform& updatedTransform = coordinator->getComponent<Transform>(entity);

        REQUIRE(updatedTransform.position[0] == Catch::Approx(1.0f));
        REQUIRE(updatedTransform.position[1] == Catch::Approx(2.0f));
        REQUIRE(updatedTransform.position[2] == Catch::Approx(3.0f));

        // Update physics for another 0.5 seconds
        physicsSystem->update(0.5f);

        REQUIRE(updatedTransform.position[0] == Catch::Approx(1.5f));
        REQUIRE(updatedTransform.position[1] == Catch::Approx(3.0f));
        REQUIRE(updatedTransform.position[2] == Catch::Approx(4.5f));
    }
}

TEST_CASE("ECS Health System", "[ECS][Health]") {
    auto coordinator = createCoordinator();

    // Register components and system
    coordinator->registerComponent<Health>();

    auto healthSystem = coordinator->registerSystem<HealthSystem>(coordinator.get());

    Signature healthSignature;
    healthSignature.set(coordinator->getComponentType<Health>());
    coordinator->setSystemSignature<HealthSystem>(healthSignature);

    SECTION("Health component functionality") {
        Health health{ 100.0f };

        REQUIRE(health.isAlive());
        REQUIRE(health.getHealthPercentage() == Catch::Approx(1.0f));

        health.takeDamage(30.0f);
        REQUIRE(health.current == Catch::Approx(70.0f));
        REQUIRE(health.getHealthPercentage() == Catch::Approx(0.7f));
        REQUIRE(health.isAlive());

        health.heal(20.0f);
        REQUIRE(health.current == Catch::Approx(90.0f));
        REQUIRE(health.isAlive());

        health.takeDamage(100.0f);
        REQUIRE(health.current == Catch::Approx(0.0f));
        REQUIRE_FALSE(health.isAlive());
    }

    SECTION("Entity destruction on death") {
        Entity entity = coordinator->createEntity();
        coordinator->addComponent(entity, Health{ 50.0f });

        REQUIRE(coordinator->getLivingEntityCount() == 1);
        REQUIRE(healthSystem->getEntityCount() == 1);

        // Damage the entity to death
        healthSystem->damageEntity(entity, 60.0f);

        // Update the health system - should destroy the entity
        healthSystem->update(0.016f);

        REQUIRE(coordinator->getLivingEntityCount() == 0);
        REQUIRE(healthSystem->getEntityCount() == 0);
    }
}

TEST_CASE("ECS Component Array Functionality", "[ECS][ComponentArray]") {
    ComponentArray<Transform> transformArray;

    SECTION("Basic insertion and retrieval") {
        Entity entity1 = 1;
        Entity entity2 = 2;

        Transform transform1{ math::Vec3f{1.0f, 2.0f, 3.0f} };
        Transform transform2{ math::Vec3f{4.0f, 5.0f, 6.0f} };

        transformArray.insertData(entity1, transform1);
        transformArray.insertData(entity2, transform2);

        REQUIRE(transformArray.getSize() == 2);
        REQUIRE(transformArray.hasData(entity1));
        REQUIRE(transformArray.hasData(entity2));

        Transform& retrieved1 = transformArray.getData(entity1);
        Transform& retrieved2 = transformArray.getData(entity2);

        REQUIRE(retrieved1.position[0] == Catch::Approx(1.0f));
        REQUIRE(retrieved2.position[0] == Catch::Approx(4.0f));
    }

    SECTION("Swap and pop removal") {
        Entity entity1 = 1;
        Entity entity2 = 2;
        Entity entity3 = 3;

        Transform transform1{ math::Vec3f{1.0f, 0.0f, 0.0f} };
        Transform transform2{ math::Vec3f{2.0f, 0.0f, 0.0f} };
        Transform transform3{ math::Vec3f{3.0f, 0.0f, 0.0f} };

        transformArray.insertData(entity1, transform1);
        transformArray.insertData(entity2, transform2);
        transformArray.insertData(entity3, transform3);

        REQUIRE(transformArray.getSize() == 3);

        // Remove middle entity
        transformArray.removeData(entity2);

        REQUIRE(transformArray.getSize() == 2);
        REQUIRE(transformArray.hasData(entity1));
        REQUIRE_FALSE(transformArray.hasData(entity2));
        REQUIRE(transformArray.hasData(entity3));

        // Remaining entities should still have correct data
        REQUIRE(transformArray.getData(entity1).position[0] == Catch::Approx(1.0f));
        REQUIRE(transformArray.getData(entity3).position[0] == Catch::Approx(3.0f));
    }

    SECTION("Entity destroyed notification") {
        Entity entity1 = 1;
        Entity entity2 = 2;

        transformArray.insertData(entity1, Transform{});
        transformArray.insertData(entity2, Transform{});

        REQUIRE(transformArray.getSize() == 2);

        // Simulate entity destruction
        transformArray.entityDestroyed(entity1);

        REQUIRE(transformArray.getSize() == 1);
        REQUIRE_FALSE(transformArray.hasData(entity1));
        REQUIRE(transformArray.hasData(entity2));

        // Entity that wasn't in array should not affect anything
        transformArray.entityDestroyed(999);
        REQUIRE(transformArray.getSize() == 1);
    }
}

TEST_CASE("ECS Integration Test", "[ECS][Integration]") {
    auto coordinator = createCoordinator();

    // Register all components
    coordinator->registerComponent<Transform>();
    coordinator->registerComponent<Velocity>();
    coordinator->registerComponent<Renderable>();
    coordinator->registerComponent<Health>();

    // Register all systems
    auto physicsSystem = coordinator->registerSystem<PhysicsSystem>(coordinator.get());
    auto renderSystem = coordinator->registerSystem<RenderSystem>(coordinator.get());
    auto healthSystem = coordinator->registerSystem<HealthSystem>(coordinator.get());

    // Set system signatures
    Signature physicsSignature;
    physicsSignature.set(coordinator->getComponentType<Transform>());
    physicsSignature.set(coordinator->getComponentType<Velocity>());
    coordinator->setSystemSignature<PhysicsSystem>(physicsSignature);

    Signature renderSignature;
    renderSignature.set(coordinator->getComponentType<Transform>());
    renderSignature.set(coordinator->getComponentType<Renderable>());
    coordinator->setSystemSignature<RenderSystem>(renderSignature);

    Signature healthSignature;
    healthSignature.set(coordinator->getComponentType<Health>());
    coordinator->setSystemSignature<HealthSystem>(healthSignature);

    SECTION("Complex entity with multiple systems") {
        Entity entity = coordinator->createEntity();

        // Add all components
        coordinator->addComponent(entity, Transform{ math::Vec3f{0.0f, 0.0f, 0.0f} });
        coordinator->addComponent(entity, Velocity{ math::Vec3f{1.0f, 0.0f, 0.0f} });
        coordinator->addComponent(entity, Renderable{});
        coordinator->addComponent(entity, Health{ 100.0f });

        // Entity should be in all systems
        REQUIRE(physicsSystem->hasEntity(entity));
        REQUIRE(renderSystem->hasEntity(entity));
        REQUIRE(healthSystem->hasEntity(entity));

        // Update all systems
        coordinator->updateSystems(1.0f);

        // Verify physics updated position
        Transform& transform = coordinator->getComponent<Transform>(entity);
        REQUIRE(transform.position[0] == Catch::Approx(1.0f));

        // Kill the entity
        healthSystem->damageEntity(entity, 150.0f);
        coordinator->updateSystems(0.016f);

        // Entity should be destroyed and removed from all systems
        REQUIRE(coordinator->getLivingEntityCount() == 0);
        REQUIRE(physicsSystem->getEntityCount() == 0);
        REQUIRE(renderSystem->getEntityCount() == 0);
        REQUIRE(healthSystem->getEntityCount() == 0);
    }
}

#include "Scene2D.h"
#include "SceneManager.h"

namespace scene {

    void Scene2D::initialize2D(SceneManager& manager) {
        // Create Renderer2D instance for this scene
        auto* resourceManager = getResourceManager(manager);
        if (!resourceManager) {
            std::cerr << "[Scene2D] Warning: No resource manager available" << std::endl;
            return;
        }

        // Get render queue builder from scene manager
        RenderQueueBuilder& renderBuilder = manager.getRenderQueueBuilder();

        renderer2D = createRenderer2D(*resourceManager, renderBuilder);

        // Configure Renderer2D
        RendererConfig2D config;
        config.maxQuadsPerBatch = 10000;
        config.enableBatching = true;
        config.enableSorting = true;

        if (!renderer2D->init(config)) {
            std::cerr << "[Scene2D] Failed to initialize Renderer2D!" << std::endl;
            return;
        }

        // Register ECS systems for 2D rendering
        auto* coord = getCoordinator();
        if (!coord) {
            std::cerr << "[Scene2D] Warning: No ECS coordinator available" << std::endl;
            return;
        }

        // Register Renderer2D system (for converting Transform+Renderable2D to render commands)
        auto renderSystemPtr = coord->registerSystem<ecs::systems::Renderer2DSystem>(coord, renderer2D.get());
        renderSystem2D = renderSystemPtr; // shared_ptr assignment

        // Register physics system for Transform+Velocity movement
        if (!physicsSystem) {
            auto physicsSystemPtr = coord->registerSystem<ecs::systems::PhysicsSystem>(coord);
            physicsSystem = physicsSystemPtr; // shared_ptr assignment

            // Set physics signature (Transform + Velocity)
            ecs::Signature physicsSignature;
            physicsSignature.set(coord->getComponentType<ecs::components::Transform>());
            physicsSignature.set(coord->getComponentType<ecs::components::Velocity>());
            coord->setSystemSignature<ecs::systems::PhysicsSystem>(physicsSignature);
        }

        // Register health system for damage/health management
        if (!healthSystem) {
            auto healthSystemPtr = coord->registerSystem<ecs::systems::HealthSystem>(coord);
            healthSystem = healthSystemPtr; // shared_ptr assignment

            // Set health signature (Health component only)
            ecs::Signature healthSignature;
            healthSignature.set(coord->getComponentType<ecs::components::Health>());
            coord->setSystemSignature<ecs::systems::HealthSystem>(healthSignature);
        }

        // Set render signature (Transform + Renderable2D)
        ecs::Signature renderSignature;
        renderSignature.set(coord->getComponentType<ecs::components::Transform>());
        renderSignature.set(coord->getComponentType<ecs::components::Renderable2D>());
        coord->setSystemSignature<ecs::systems::Renderer2DSystem>(renderSignature);

        std::cout << "[Scene2D] Renderer2D and ECS systems initialized successfully" << std::endl;
    }

} // namespace scene

#include "Scene2D.h"
#include <iostream>

namespace scene {

    void Scene2D::initialize2D(SceneManager& manager) {
        auto* coord = getCoordinator();
        if (!coord) return;

        // Ottieni resource manager e render queue builder
        const auto& resMgr = manager.getResourceManager();
        auto& builder = manager.getRenderQueueBuilder();

        // Crea l'istanza concreta del Renderer2D
        renderer2D = std::make_unique<Renderer2DImpl>(*resMgr, builder);

        // Configura e inizializza il renderer
        RendererConfig2D config;
        config.maxQuadsPerBatch = 10000;
        config.enableBatching = true;
        config.enableSorting = true;

        if (!renderer2D->init(config)) {
            std::cerr << "[Scene2D] Failed to initialize Renderer2D!" << std::endl;
            renderer2D.reset();
            return;
        }

        // Registra i sistemi ECS comuni
        // PhysicsSystem
        {
            auto sys = coord->registerSystem<ecs::systems::PhysicsSystem>(coord);
            ecs::Signature sig;
            sig.set(coord->getComponentType<ecs::components::Transform>());
            sig.set(coord->getComponentType<ecs::components::Velocity>());
            coord->setSystemSignature<ecs::systems::PhysicsSystem>(sig);
        }
        // HealthSystem
        {
            auto sys = coord->registerSystem<ecs::systems::HealthSystem>(coord);
            ecs::Signature sig;
            sig.set(coord->getComponentType<ecs::components::Health>());
            coord->setSystemSignature<ecs::systems::HealthSystem>(sig);
        }
        // Renderer2DSystem
        {
            auto sys = coord->registerSystem<ecs::systems::Renderer2DSystem>(coord, renderer2D.get());
            renderer2DSystem = sys.get();  // Save raw pointer for camera management

            ecs::Signature sig;
            sig.set(coord->getComponentType<ecs::components::Transform>());
            sig.set(coord->getComponentType<ecs::components::Renderable2D>());
            coord->setSystemSignature<ecs::systems::Renderer2DSystem>(sig);

            // Setup default scene camera
            sceneCamera.setPosition({ 0.f, 0.f });
            sceneCamera.setZoom(1.0f);
            sceneCamera.setViewportSize({ 1920.0f, 1080.0f }); // Default viewport

            // Set the camera in the system
            renderer2DSystem->setActiveCamera(&sceneCamera);
        }
    }

    void Scene2D::render(RenderQueueBuilder& builder) {
        if (!renderer2D || !renderer2DSystem) return;

        // The Renderer2DSystem now handles beginScene/endScene internally through the facade
        // We just need to call the custom render method for any additional rendering
        render2DCustom();

        // Note: ECS systems are automatically called by the Scene base class
        // The Renderer2DSystem will collect all entities and render them via the facade
    }

    ecs::Entity Scene2D::createSprite(const math::Vec2f& pos,
        const math::Vec2f& size,
        const std::string& tex,
        const scene::Color& tint,
        uint32_t layer) {
        auto* coord = getCoordinator();
        if (!coord) return 0;

        auto e = coord->createEntity();

        ecs::components::Transform t;
        t.position = { pos.x(), pos.y(), static_cast<float>(layer) };
        t.scale = { size.x(), size.y(), 1.f };
        coord->addComponent(e, t);

        ecs::components::Renderable2D r;
        r.textureId = tex;
        r.color = tint;
        coord->addComponent(e, r);

        return e;
    }

    ecs::Entity Scene2D::createColoredQuad(const math::Vec2f& pos,
        const math::Vec2f& size,
        const scene::Color& color,
        uint32_t layer) {
        return createSprite(pos, size, "", color, layer);
    }

} // namespace scene

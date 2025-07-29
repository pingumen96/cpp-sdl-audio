#pragma once
/*
 * Scene2D.h - Clean version, 100% ECS-oriented
 *
 * Principles:
 *  • The scene does NOT hold game logic, input, or system state.
 *  • It is limited to:
 *      - creating graphics resources (Renderer2D);
 *      - registering necessary generic ECS systems;
 *      - providing utilities to instantiate 2D entities (Transform + Renderable2D).
 *  • Debug output and system pointers have been removed:
 *      scenes should not know the execution order of systems.
 */

#include "SceneManager.h"
#include "Scene.h"
#include "rendering/Renderer2D.h"
#include "rendering/Renderer2DImpl.h"

#include "../ecs/Coordinator.h"
#include "../ecs/components/Renderable2D.h"
#include "../ecs/components/CommonComponents.h"
#include "../ecs/systems/Renderer2DSystem.h"
#include "../ecs/systems/CommonSystems.h"

namespace scene {

    class Scene2D : public Scene {
    protected:
        /*  The only resource the scene directly cares about.
            Passed to the Renderer2DSystem via raw reference
            (ECS will manage the system lifecycle).                         */
        std::unique_ptr<scene::IRenderer2D> renderer2D{};

        // Reference to the Renderer2DSystem for camera management
        ecs::systems::Renderer2DSystem* renderer2DSystem = nullptr;

        // Scene camera for 2D rendering
        scene::Camera2D sceneCamera;

    public:
        Scene2D() = default;
        ~Scene2D() override = default;

        /* INITIALIZATION: declaration */
        void initialize2D(SceneManager& manager);

        /* RENDERING */
        void render(RenderQueueBuilder& builder) override;

        /* Optional: override for special effects */
        virtual void render2DCustom() {}

        /* ENTITY HELPERS */
        ecs::Entity createSprite(const math::Vec2f& pos,
            const math::Vec2f& size,
            const std::string& tex,
            const scene::Color& tint = scene::Color::White,
            uint32_t layer = 0);

        ecs::Entity createColoredQuad(const math::Vec2f& pos,
            const math::Vec2f& size,
            const scene::Color& color,
            uint32_t layer = 0);

        scene::IRenderer2D* getRenderer2D() const { return renderer2D.get(); }

        /**
         * @brief Get access to the scene camera for modifications
         */
        scene::Camera2D& getSceneCamera() { return sceneCamera; }
        const scene::Camera2D& getSceneCamera() const { return sceneCamera; }

        /**
         * @brief Get access to the Renderer2DSystem for advanced operations
         */
        ecs::systems::Renderer2DSystem* getRenderer2DSystem() const { return renderer2DSystem; }
    };
} // namespace scene

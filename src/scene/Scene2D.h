#pragma once

#include "Scene.h"
#include "rendering/Renderer2D.h"
#include "rendering/Renderer2DImpl.h"
#include "../ecs/systems/CommonSystems.h"

namespace scene {

    /**
     * @brief Enhanced Scene base class with integrated Renderer2D and ECS systems
     *
     * This class provides a complete foundation for 2D game scenes by integrating:
     * - ECS coordinator with common systems
     * - Renderer2D for optimized 2D rendering
     * - Automatic system registration and management
     */
    class Scene2D : public Scene {
    protected:
        // Renderer2D integration
        std::unique_ptr<IRenderer2D> renderer2D;
        std::shared_ptr<ecs::systems::Renderer2DSystem> renderSystem2D; // ECS manages system lifecycle

        // Common ECS systems
        std::shared_ptr<ecs::systems::PhysicsSystem> physicsSystem;
        std::shared_ptr<ecs::systems::HealthSystem> healthSystem;

    public:
        Scene2D() = default;
        virtual ~Scene2D() = default;

        /**
         * @brief Initialize 2D scene with Renderer2D integration
         * Called automatically during scene attachment
         */
        void initialize2D(SceneManager& manager);

        /**
         * @brief Setup common ECS systems for 2D scenes
         */
        virtual void setupECSSystems() {
            auto* coord = getCoordinator();
            if (!coord) return;

            // Register systems in ECS (they are created and managed by ECS)
            auto physicsSystemPtr = coord->registerSystem<ecs::systems::PhysicsSystem>(coord);
            auto healthSystemPtr = coord->registerSystem<ecs::systems::HealthSystem>(coord);
            auto renderSystemPtr = coord->registerSystem<ecs::systems::Renderer2DSystem>(coord, renderer2D.get());

            // Keep references to systems we need to configure
            physicsSystem = physicsSystemPtr; // shared_ptr assignment
            healthSystem = healthSystemPtr;
            renderSystem2D = renderSystemPtr;

            // Set system signatures
            ecs::Signature physicsSignature;
            physicsSignature.set(coord->getComponentType<ecs::components::Transform>());
            physicsSignature.set(coord->getComponentType<ecs::components::Velocity>());
            coord->setSystemSignature<ecs::systems::PhysicsSystem>(physicsSignature);

            ecs::Signature healthSignature;
            healthSignature.set(coord->getComponentType<ecs::components::Health>());
            coord->setSystemSignature<ecs::systems::HealthSystem>(healthSignature);

            ecs::Signature render2DSignature;
            render2DSignature.set(coord->getComponentType<ecs::components::Transform>());
            render2DSignature.set(coord->getComponentType<ecs::components::Renderable2D>());
            coord->setSystemSignature<ecs::systems::Renderer2DSystem>(render2DSignature);
        }

        /**
         * @brief Default render implementation using Renderer2D
         * Subclasses can override this for custom rendering
         */
        void render(RenderQueueBuilder& builder) override {
            if (!renderer2D) {
                // Fallback: render nothing or basic shapes
                return;
            }

            // Setup 2D camera (already orthographic by default)
            Camera2D camera;
            camera.setViewportSize(math::Vec2f(800.0f, 600.0f)); // TODO: Get actual viewport size

            // Position camera to match game coordinate system (center at screen center)
            camera.setPosition(math::Vec2f(400.0f, 300.0f)); // Center camera at screen center for Pong

            // Begin 2D scene
            renderer2D->beginScene(camera);

            // Let ECS systems render (Renderer2DSystem will handle entities)
            if (renderSystem2D) {
                renderSystem2D->update(0.0f); // deltaTime not needed for rendering
            }

            // Custom rendering (override point for subclasses)
            render2DCustom();

            // End scene (this flushes to the RenderQueueBuilder)
            renderer2D->endScene();
        }

        /**
         * @brief Override point for custom 2D rendering
         * Called between beginScene/endScene
         */
        virtual void render2DCustom() {
            // Default: do nothing
            // Subclasses can override to add manual rendering
        }

        // Utility methods for entity creation

        /**
         * @brief Create a sprite entity
         */
        ecs::Entity createSprite(const math::Vec2f& position,
            const math::Vec2f& size,
            const std::string& texture,
            const scene::Color& color = scene::Color::White,
            uint32_t layer = 0) {
            if (renderSystem2D) {
                return renderSystem2D->createSpriteEntity(position, size, texture, color, layer);
            }
            return 0;
        }

        /**
         * @brief Create a colored quad entity
         */
        ecs::Entity createColoredQuad(const math::Vec2f& position,
            const math::Vec2f& size,
            const scene::Color& color,
            uint32_t layer = 0) {
            if (renderSystem2D) {
                return renderSystem2D->createQuadEntity(position, size, color, layer);
            }
            return 0;
        }

        /**
         * @brief Get Renderer2D instance
         */
        IRenderer2D* getRenderer2D() const {
            return renderer2D.get();
        }

        /**
         * @brief Get 2D render system
         */
        ecs::systems::Renderer2DSystem* getRenderSystem2D() const {
            return renderSystem2D.get();
        }
    };

} // namespace scene

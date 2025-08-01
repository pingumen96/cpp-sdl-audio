#pragma once

#include "../System.h"
#include "../../scene/rendering/Renderer2D.h"
#include "../../scene/rendering/Render2DFacade.h"
#include "../components/CommonComponents.h"
#include "../components/Renderable2D.h"
#include <memory>

namespace ecs::systems {

    /**
     * @brief ECS System for 2D rendering using Renderer2D with unified facade
     *
     * This system processes all entities with Transform and Renderable2D components
     * and submits them to the Render2DFacade for batched rendering. The facade handles
     * the actual interaction with IRenderer2D, eliminating duplication with test systems.
     */
    class Renderer2DSystem : public System {
    private:
        Coordinator* coordinator;
        scene::IRenderer2D* renderer2D;
        scene::Render2DFacade facade;
        const scene::Camera2D* activeCamera = nullptr;
        bool enabled = true;

    public:
        /**
         * @brief Constructor
         * @param coord ECS coordinator
         * @param renderer Renderer2D instance for this scene
         */
        Renderer2DSystem(Coordinator* coord, scene::IRenderer2D* renderer)
            : coordinator(coord), renderer2D(renderer) {}

        /**
         * @brief Update system - collect all 2D entities and submit to facade
         * @param deltaTime Time since last update (unused for rendering)
         */
        void update(float deltaTime) override {
            if (!renderer2D || !enabled || !coordinator || !activeCamera) {
                return;
            }

            // Clear any previous frame's requests
            facade.clear();

            // Process all entities with Transform and Renderable2D components
            for (auto const& entity : mEntities) {
                auto& transform = coordinator->getComponent<ecs::components::Transform>(entity);
                auto& renderable = coordinator->getComponent<ecs::components::Renderable2D>(entity);

                // Skip invisible entities
                if (!renderable.visible) {
                    continue;
                }

                // Create rectangle from transform and renderable data
                scene::Rect2D rect;
                rect.position = math::Vec2f(transform.position.x(), transform.position.y()); // Convert Vec3f to Vec2f
                rect.size = renderable.size;

                // Apply transform scale to size if needed
                rect.size.x() *= transform.scale.x();
                rect.size.y() *= transform.scale.y();

                // Submit to facade instead of directly to renderer
                facade.submit(
                    rect,
                    renderable.color,
                    renderable.textureId,
                    renderable.layer,
                    0.0f,  // rotation (using transform.rotation in future)
                    renderable.depth
                );
            }

            // Flush all collected requests to the renderer
            facade.flush(*renderer2D, *activeCamera);
        }

        /**
         * @brief Set the active camera for rendering
         * @param camera Camera to use for rendering (must remain valid during system lifetime)
         */
        void setActiveCamera(const scene::Camera2D* camera) {
            activeCamera = camera;
        }

        /**
         * @brief Get the current active camera
         * @return Pointer to the active camera, or nullptr if none set
         */
        const scene::Camera2D* getActiveCamera() const {
            return activeCamera;
        }

        /**
         * @brief Enable/disable the system
         */
        void setEnabled(bool enable) {
            enabled = enable;
        }

        /**
         * @brief Check if system is enabled
         */
        bool isEnabled() const {
            return enabled;
        }

        /**
         * @brief Set the renderer2D instance (useful for scene switching)
         */
        void setRenderer2D(scene::IRenderer2D* renderer) {
            renderer2D = renderer;
        }

        /**
         * @brief Get current renderer2D instance
         */
        scene::IRenderer2D* getRenderer2D() const {
            return renderer2D;
        }

        /**
         * @brief Get access to the facade for advanced usage
         */
        const scene::Render2DFacade& getFacade() const {
            return facade;
        }

        /**
         * @brief Get facade statistics
         */
        uint32_t getLastFrameQuadCount() const {
            return facade.getLastFrameQuadCount();
        }

        // Utility methods for common entity creation

        /**
         * @brief Create a sprite entity with Transform and Renderable2D
         * @param position Sprite position (2D)
         * @param size Sprite size
         * @param texture Texture identifier
         * @param color Sprite tint color
         * @param layer Rendering layer
         * @return Created entity ID
         */
        ecs::Entity createSpriteEntity(const math::Vec2f& position,
            const math::Vec2f& size,
            const std::string& texture,
            const scene::Color& color = scene::Color::White,
            uint32_t layer = 0) {
            if (!coordinator) return 0;

            auto entity = coordinator->createEntity();

            // Add Transform component
            components::Transform transform;
            transform.position = math::Vec3f(position.x(), position.y(), 0.0f); // Convert Vec2f to Vec3f
            transform.scale = math::Vec3f(1.0f, 1.0f, 1.0f);
            transform.rotation = math::Quatf(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
            coordinator->addComponent(entity, transform);

            // Add Renderable2D component
            auto renderable = components::Renderable2D::createSprite(texture, size, layer);
            renderable.color = color;
            coordinator->addComponent(entity, renderable);

            return entity;
        }

        /**
         * @brief Create a colored quad entity
         * @param position Quad position (2D)
         * @param size Quad size
         * @param color Quad color
         * @param layer Rendering layer
         * @return Created entity ID
         */
        ecs::Entity createQuadEntity(const math::Vec2f& position,
            const math::Vec2f& size,
            const scene::Color& color,
            uint32_t layer = 0) {
            if (!coordinator) return 0;

            auto entity = coordinator->createEntity();

            // Add Transform component
            components::Transform transform;
            transform.position = math::Vec3f(position.x(), position.y(), 0.0f); // Convert Vec2f to Vec3f
            transform.scale = math::Vec3f(1.0f, 1.0f, 1.0f);
            transform.rotation = math::Quatf(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
            coordinator->addComponent(entity, transform);

            // Add Renderable2D component
            auto renderable = components::Renderable2D::createColoredQuad(color, size, layer);
            coordinator->addComponent(entity, renderable);

            return entity;
        }
    };

} // namespace ecs::systems

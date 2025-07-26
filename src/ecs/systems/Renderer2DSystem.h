#pragma once

#include "../System.h"
#include "../../scene/rendering/Renderer2D.h"
#include "../components/CommonComponents.h"
#include "../components/Renderable2D.h"
#include <memory>

namespace ecs::systems {

    /**
     * @brief ECS System for 2D rendering using Renderer2D
     *
     * This system processes all entities with Transform and Renderable2D components
     * and submits them to the Renderer2D for batched rendering.
     */
    class Renderer2DSystem : public System {
    private:
        Coordinator* coordinator;
        scene::IRenderer2D* renderer2D;
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
         * @brief Update system - render all 2D entities
         * @param deltaTime Time since last update (unused for rendering)
         */
        void update(float deltaTime) override {
            if (!renderer2D || !enabled || !coordinator) {
                return;
            }

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

                // Submit to renderer (note: base interface doesn't support rotation yet)
                renderer2D->drawRect(
                    rect,
                    renderable.color,
                    renderable.textureId
                );
            }
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

        // Utility methods for common entity creation

        /**
         * @brief Create a sprite entity with Transform and Renderable2D
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

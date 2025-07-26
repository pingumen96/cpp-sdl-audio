#pragma once

#include "../../math/math.h"
#include "../../scene/rendering/Renderer2D.h"
#include <string>

namespace ecs::components {

    /**
     * @brief Component for 2D rendering using Renderer2D system
     *
     * This component contains all data needed for 2D sprite/quad rendering.
     * It works in conjunction with Transform component to define position/scale/rotation.
     */
    struct Renderable2D {
        // Visual properties
        scene::Color color = scene::Color::White;
        std::string textureId = "";

        // Rendering properties
        uint32_t layer = 0;
        float depth = 0.0f;
        bool visible = true;

        // UV coordinates (for texture atlasing/sprites)
        math::Vec2f uvMin = math::Vec2f(0.0f, 0.0f);
        math::Vec2f uvMax = math::Vec2f(1.0f, 1.0f);

        // Size (if not using Transform scale)
        math::Vec2f size = math::Vec2f(100.0f, 100.0f);

        // Material/shader overrides (optional)
        std::string materialId = "";

        // Constructor
        Renderable2D() = default;

        Renderable2D(const scene::Color& col, const std::string& tex = "", uint32_t renderLayer = 0)
            : color(col), textureId(tex), layer(renderLayer) {}

        // Helper constructors for common cases
        static Renderable2D createSprite(const std::string& texture, const math::Vec2f& spriteSize, uint32_t layer = 0) {
            Renderable2D renderable;
            renderable.textureId = texture;
            renderable.size = spriteSize;
            renderable.layer = layer;
            renderable.color = scene::Color::White;
            return renderable;
        }

        static Renderable2D createColoredQuad(const scene::Color& color, const math::Vec2f& quadSize, uint32_t layer = 0) {
            Renderable2D renderable;
            renderable.color = color;
            renderable.size = quadSize;
            renderable.layer = layer;
            renderable.textureId = ""; // No texture, just color
            return renderable;
        }
    };

} // namespace ecs::components

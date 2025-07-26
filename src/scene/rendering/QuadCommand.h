#pragma once

#include "../../math/math.h"
#include "../../resources/ResourceManager.h"
#include <vector>

namespace scene {

    /**
     * @brief Comando per disegnare un quad 2D
     *
     * Struttura POD che contiene tutti i dati necessari per disegnare
     * un quadrilatero 2D. Viene usata come "moneta" tra gameplay,
     * batcher e RenderQueueBuilder.
     */
    struct QuadCommand {
        math::Matrix4f transform{ 1.0f };       ///< Matrice di trasformazione 3D
        math::Vec4f color{ 1.0f };              ///< Colore RGBA
        std::string textureId;                  ///< Handle texture (vuoto per solid color)
        math::Vec2f uvOffset{ 0.0f };           ///< Offset UV per atlas/tiling
        math::Vec2f uvScale{ 1.0f };            ///< Scale UV per atlas/tiling
        uint32_t layer = 0;                     ///< Layer di sorting
        float depth = 0.0f;                     ///< Profondità per sorting

        QuadCommand() = default;

        QuadCommand(const math::Matrix4f& trans, const math::Vec4f& col,
            const std::string& tex = "", uint32_t lay = 0, float d = 0.0f)
            : transform(trans), color(col), textureId(tex), layer(lay), depth(d) {}

        /**
         * @brief Factory per creare comando da rettangolo e colore
         */
        static QuadCommand fromRect(const math::Vec2f& position, const math::Vec2f& size,
            const math::Vec4f& color, const std::string& texture = "",
            float rotation = 0.0f, uint32_t layer = 0, float depth = 0.0f) {
            // Crea matrice di trasformazione: T * R * S
            math::Matrix4f translation = math::Matrix4f::translation(position.x(), position.y(), 0.0f);
            math::Matrix4f scale = math::Matrix4f::scale(size.x(), size.y(), 1.0f);
            math::Matrix4f rotate = math::Matrix4f::rotationZ(rotation);

            math::Matrix4f transform = translation * rotate * scale;

            return QuadCommand(transform, color, texture, layer, depth);
        }

        /**
         * @brief Verifica se due comandi sono "batchabili" insieme
         */
        bool isBatchCompatible(const QuadCommand& other) const {
            return textureId == other.textureId && layer == other.layer;
        }
    };

} // namespace scene

#pragma once

#include "../../resources/ResourceManager.h"
#include <string>
#include <memory>

namespace scene {

    /**
     * @brief Handle per shader
     */
    using ShaderHandle = std::string;

    /**
     * @brief Materiale 2D che combina shader e parametri uniform
     *
     * Descrive l'accoppiata shader + parametri uniform per il rendering 2D.
     * È condiviso tra più quads per ridurre i cambi di stato.
     */
    class Material2D {
    private:
        ShaderHandle shaderHandle;
        std::string textureHandle;
        math::Vec4f baseColor{ 1.0f };
        math::Vec2f uvOffset{ 0.0f };
        math::Vec2f uvScale{ 1.0f };
        float opacity = 1.0f;

    public:
        Material2D() = default;
        Material2D(const ShaderHandle& shader) : shaderHandle(shader) {}
        Material2D(const ShaderHandle& shader, const std::string& texture)
            : shaderHandle(shader), textureHandle(texture) {}

        // Getters
        const ShaderHandle& getShader() const { return shaderHandle; }
        const std::string& getTexture() const { return textureHandle; }
        const math::Vec4f& getBaseColor() const { return baseColor; }
        const math::Vec2f& getUVOffset() const { return uvOffset; }
        const math::Vec2f& getUVScale() const { return uvScale; }
        float getOpacity() const { return opacity; }

        // Setters
        void setShader(const ShaderHandle& shader) { shaderHandle = shader; }
        void setTexture(const std::string& texture) { textureHandle = texture; }
        void setBaseColor(const math::Vec4f& color) { baseColor = color; }
        void setUVOffset(const math::Vec2f& offset) { uvOffset = offset; }
        void setUVScale(const math::Vec2f& scale) { uvScale = scale; }
        void setOpacity(float op) { opacity = op; }

        /**
         * @brief Verifica se due materiali sono compatibili per batching
         */
        bool isBatchCompatible(const Material2D& other) const {
            return shaderHandle == other.shaderHandle &&
                textureHandle == other.textureHandle;
        }

        /**
         * @brief Genera un ID univoco per il materiale (per batching)
         */
        std::string getBatchId() const {
            return shaderHandle + "|" + textureHandle;
        }

        /**
         * @brief Factory methods per materiali comuni
         */
        static Material2D createSolidColor(const math::Vec4f& color = math::Vec4f(1.0f)) {
            Material2D material("flat_color");
            material.setBaseColor(color);
            return material;
        }

        static Material2D createTextured(const std::string& texture,
            const math::Vec4f& tint = math::Vec4f(1.0f)) {
            Material2D material("texture", texture);
            material.setBaseColor(tint);
            return material;
        }

        static Material2D createTexturedTinted(const std::string& texture,
            const math::Vec4f& tint) {
            Material2D material("texture_tinted", texture);
            material.setBaseColor(tint);
            return material;
        }
    };

    using Material2DPtr = std::shared_ptr<Material2D>;

} // namespace scene

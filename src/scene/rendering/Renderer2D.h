#pragma once

#include "../../math/math.h"
#include "../../resources/ResourceManager.h"
#include "IRenderBackend.h"
#include "RenderQueueBuilder.h"
#include <memory>
#include <string>

namespace scene {

    // Forward declarations
    class QuadBatch;
    class Material2D;
    class Shader2DLibrary;
    class VertexCache2D;

    /**
     * @brief 2D rendering configuration
     */
    struct RendererConfig2D {
        uint32_t maxQuadsPerBatch = 1000;       ///< Maximum quads per batch
        bool enableBatching = true;             ///< Enable automatic batching
        bool enableSorting = true;              ///< Enable depth sorting
        std::string defaultShader = "flat_color"; ///< Default shader name

        RendererConfig2D() = default;
    };

    /**
     * @brief Handle per texture 2D
     */
    using TextureHandle = std::string;

    /**
     * @brief Semplice struttura rettangolo 2D per il rendering
     */
    struct Rect2D {
        math::Vec2f position{ 0.0f };
        math::Vec2f size{ 1.0f };

        Rect2D() = default;
        Rect2D(const math::Vec2f& pos, const math::Vec2f& sz)
            : position(pos), size(sz) {}
        Rect2D(float x, float y, float w, float h)
            : position(x, y), size(w, h) {}
    };

    /**
     * @brief Colore RGBA
     */
    struct Color {
        float r, g, b, a;

        Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        Color(float red, float green, float blue, float alpha = 1.0f)
            : r(red), g(green), b(blue), a(alpha) {}

        math::Vec4f toVec4() const { return math::Vec4f(r, g, b, a); }

        // Colori predefiniti
        static const Color White;
        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Magenta;
        static const Color Cyan;
        static const Color Transparent;
    };

    /**
     * @brief Camera 2D per proiezione ortogonale
     */
    class Camera2D {
    private:
        math::Vec2f position{ 0.0f };
        float zoom = 1.0f;
        float rotation = 0.0f;
        math::Vec2f viewportSize{ 1920.0f, 1080.0f };

        mutable math::Matrix4f viewMatrix{ 1.0f };
        mutable math::Matrix4f projectionMatrix{ 1.0f };
        mutable bool viewDirty = true;
        mutable bool projectionDirty = true;

    public:
        Camera2D() = default;
        Camera2D(const math::Vec2f& pos, float z = 1.0f)
            : position(pos), zoom(z) {}

        // Setters
        void setPosition(const math::Vec2f& pos) {
            position = pos;
            viewDirty = true;
        }
        void setZoom(float z) {
            zoom = z;
            viewDirty = true;
        }
        void setRotation(float rot) {
            rotation = rot;
            viewDirty = true;
        }
        void setViewportSize(const math::Vec2f& size) {
            viewportSize = size;
            projectionDirty = true;
        }

        // Getters
        const math::Vec2f& getPosition() const { return position; }
        float getZoom() const { return zoom; }
        float getRotation() const { return rotation; }
        const math::Vec2f& getViewportSize() const { return viewportSize; }

        // Matrici (lazy calculation)
        const math::Matrix4f& getViewMatrix() const {
            if (viewDirty) {
                updateViewMatrix();
                viewDirty = false;
            }
            return viewMatrix;
        }

        const math::Matrix4f& getProjectionMatrix() const {
            if (projectionDirty) {
                updateProjectionMatrix();
                projectionDirty = false;
            }
            return projectionMatrix;
        }

        math::Matrix4f getViewProjectionMatrix() const {
            return getProjectionMatrix() * getViewMatrix();
        }

        // Utility
        void follow(const math::Vec2f& target, float lerpFactor = 1.0f) {
            setPosition(math::Vec2f(
                position.x() + (target.x() - position.x()) * lerpFactor,
                position.y() + (target.y() - position.y()) * lerpFactor
            ));
        }

    private:
        void updateViewMatrix() const {
            // View = T * R * S
            math::Matrix4f translation = math::Matrix4f::translation(-position.x(), -position.y(), 0.0f);
            math::Matrix4f scale = math::Matrix4f::scale(zoom, zoom, 1.0f);
            math::Matrix4f rotate = math::Matrix4f::rotationZ(-rotation);

            viewMatrix = scale * rotate * translation;
        }

        void updateProjectionMatrix() const {
            float halfWidth = viewportSize.x() * 0.5f;
            float halfHeight = viewportSize.y() * 0.5f;
            projectionMatrix = math::Matrix4f::ortho(
                -halfWidth, halfWidth,
                -halfHeight, halfHeight,
                -1.0f, 1.0f
            );
        }
    };

    /**
     * @brief Interfaccia per il renderer 2D
     */
    class IRenderer2D {
    public:
        virtual ~IRenderer2D() = default;

        /**
         * @brief Inizializza il renderer
         */
        virtual bool init(const RendererConfig2D& config) = 0;

        /**
         * @brief Inizia una nuova scena
         */
        virtual void beginScene(const Camera2D& camera) = 0;

        /**
         * @brief Disegna un rettangolo
         */
        virtual void drawRect(const Rect2D& rect, const Color& color,
            const TextureHandle& texture = "") = 0;

        /**
         * @brief Disegna un rettangolo con supporto per layer e rotazione
         */
        virtual void drawRect(const Rect2D& rect, const Color& color, float rotation,
            const TextureHandle& texture = "", uint32_t layer = 0, float depth = 0.0f) = 0;

        /**
         * @brief Termina la scena e invia i comandi al batch
         */
        virtual void endScene() = 0;

        /**
         * @brief Shutdown del renderer
         */
        virtual void shutdown() = 0;

        /**
         * @brief Ottieni statistiche di rendering
         */
        struct Stats {
            uint32_t quadCount = 0;
            uint32_t batchCount = 0;
            uint32_t textureBinds = 0;
        };
        virtual Stats getStats() const = 0;
        virtual void resetStats() = 0;
    };

    using IRenderer2DPtr = std::unique_ptr<IRenderer2D>;

} // namespace scene

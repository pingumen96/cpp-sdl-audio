#pragma once

#include "../backend/RenderBackend.h"
#include "../backend/CommandBuffer.h"
#include "../../math/Matrix.h"
#include "../../math/math.h"
#include <memory>
#include <vector>

namespace rendering::twod {

    /**
     * @brief 2D rendering configuration
     */
    struct Renderer2DConfig {
        uint32_t maxQuadsPerBatch = 1000;
        bool enableBatching = true;
        bool enableDepthSorting = true;
        float nearPlane = -1.0f;
        float farPlane = 1.0f;

        Renderer2DConfig() = default;
    };

    /**
     * @brief 2D rectangle structure
     */
    struct Rect2D {
        math::Vec2f position{ 0.0f };
        math::Vec2f size{ 1.0f };

        Rect2D() = default;
        Rect2D(const math::Vec2f& pos, const math::Vec2f& sz) : position(pos), size(sz) {}
        Rect2D(float x, float y, float w, float h) : position(x, y), size(w, h) {}
    };

    /**
     * @brief RGBA color structure
     */
    struct Color {
        float r, g, b, a;

        Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
            : r(red), g(green), b(blue), a(alpha) {}

        static const Color WHITE;
        static const Color BLACK;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color TRANSPARENT;
    };

    /**
     * @brief 2D drawable item for batching
     */
    struct DrawItem2D {
        Rect2D rect;
        Color color;
        backend::TextureHandle texture = 0;
        float depth = 0.0f;
        bool hasTexture = false;

        DrawItem2D() = default;

        // Colored quad constructor
        DrawItem2D(const Rect2D& r, const Color& c, float d = 0.0f)
            : rect(r), color(c), depth(d), hasTexture(false) {}

        // Textured quad constructor
        DrawItem2D(const Rect2D& r, backend::TextureHandle tex, float d = 0.0f)
            : rect(r), color(Color::WHITE), texture(tex), depth(d), hasTexture(true) {}
    };

    /**
     * @brief High-level 2D renderer
     *
     * This class provides a simple, efficient 2D rendering API. It handles
     * batching, sorting, and coordinate system transformations while delegating
     * actual rendering to the backend layer.
     */
    class Renderer2D {
    private:
        std::unique_ptr<backend::RenderBackend> backend;
        Renderer2DConfig config;

        // Camera/view state
        math::Mat4 projectionMatrix;
        math::Mat4 viewMatrix;
        bool projectionDirty = true;
        bool viewDirty = true;

        // Camera parameters
        float cameraX = 0.0f, cameraY = 0.0f;
        float viewportWidth = 800.0f, viewportHeight = 600.0f;
        float zoom = 1.0f;

        // Batch state
        std::vector<DrawItem2D> currentBatch;
        bool batchStarted = false;

        // Command buffer for immediate mode
        backend::CommandBuffer commandBuffer;

    public:
        /**
         * @brief Constructor
         * @param renderBackend Backend to use for rendering
         * @param config Configuration parameters
         */
        explicit Renderer2D(std::unique_ptr<backend::RenderBackend> renderBackend,
            const Renderer2DConfig& config = Renderer2DConfig{});

        /**
         * @brief Destructor
         */
        ~Renderer2D();

        // Prevent copy, allow move
        Renderer2D(const Renderer2D&) = delete;
        Renderer2D& operator=(const Renderer2D&) = delete;
        Renderer2D(Renderer2D&&) = default;
        Renderer2D& operator=(Renderer2D&&) = default;

        // ===== LIFECYCLE =====

        /**
         * @brief Initialize the renderer
         * @param width Initial viewport width
         * @param height Initial viewport height
         * @return true if initialization succeeded
         */
        bool initialize(uint32_t width, uint32_t height);

        /**
         * @brief Shutdown the renderer
         */
        void shutdown();

        // ===== FRAME MANAGEMENT =====

        /**
         * @brief Begin a new frame
         */
        void beginFrame();

        /**
         * @brief End the current frame and present
         */
        void endFrame();

        // ===== CAMERA/VIEW =====

        /**
         * @brief Set camera position
         */
        void setCameraPosition(float x, float y);

        /**
         * @brief Get camera position
         */
        void getCameraPosition(float& x, float& y) const;

        /**
         * @brief Set camera zoom
         */
        void setCameraZoom(float zoom);

        /**
         * @brief Get camera zoom
         */
        float getCameraZoom() const { return zoom; }

        /**
         * @brief Set viewport size (updates projection matrix)
         */
        void setViewportSize(float width, float height);

        /**
         * @brief Get viewport size
         */
        void getViewportSize(float& width, float& height) const;

        // ===== IMMEDIATE MODE RENDERING =====

        /**
         * @brief Clear the screen
         */
        void clear(const Color& color = Color::BLACK);

        /**
         * @brief Draw a colored rectangle
         * @param rect Rectangle to draw
         * @param color Rectangle color
         */
        void drawRect(const Rect2D& rect, const Color& color);

        /**
         * @brief Draw a textured rectangle
         * @param rect Rectangle to draw
         * @param texture Texture to use
         */
        void drawTexturedRect(const Rect2D& rect, backend::TextureHandle texture);

        /**
         * @brief Draw a colored quad with custom transform
         * @param transform Transform matrix
         * @param color Quad color
         */
        void drawQuad(const math::Mat4& transform, const Color& color);

        /**
         * @brief Draw a textured quad with custom transform
         * @param transform Transform matrix
         * @param texture Texture to use
         */
        void drawTexturedQuad(const math::Mat4& transform, backend::TextureHandle texture);

        // ===== BATCHED RENDERING =====

        /**
         * @brief Begin batch rendering (more efficient for many objects)
         */
        void beginBatch();

        /**
         * @brief Add item to current batch
         */
        void batchRect(const Rect2D& rect, const Color& color, float depth = 0.0f);

        /**
         * @brief Add textured item to current batch
         */
        void batchTexturedRect(const Rect2D& rect, backend::TextureHandle texture, float depth = 0.0f);

        /**
         * @brief End batch and submit for rendering
         */
        void endBatch();

        // ===== RESOURCE HELPERS =====

        /**
         * @brief Create texture from data
         */
        backend::TextureHandle createTexture(const unsigned char* data, int width, int height,
            int channels, const std::string& name = "");

        /**
         * @brief Delete texture
         */
        void deleteTexture(backend::TextureHandle texture);

        // ===== INFORMATION =====

        /**
         * @brief Get renderer information
         */
        std::string getRendererInfo() const;

        /**
         * @brief Get configuration
         */
        const Renderer2DConfig& getConfig() const { return config; }

        /**
         * @brief Get backend for advanced usage
         */
        backend::RenderBackend* getBackend() const { return backend.get(); }

    private:
        void updateProjectionMatrix();
        void updateViewMatrix();
        void flushBatch();
        math::Mat4 createTransformMatrix(const Rect2D& rect) const;
        void sortBatchByDepth();
    };

    // Color constants
    inline const Color Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
    inline const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
    inline const Color Color::RED(1.0f, 0.0f, 0.0f, 1.0f);
    inline const Color Color::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
    inline const Color Color::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
    inline const Color Color::TRANSPARENT(0.0f, 0.0f, 0.0f, 0.0f);

} // namespace rendering::twod

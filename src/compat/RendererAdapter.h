#pragma once

/**
 * @file RendererAdapter.h
 * @brief Compatibility adapter for the old Renderer interface
 *
 * This adapter maintains backward compatibility with existing code that uses
 * the old core::Renderer interface while internally using the new stratified
 * rendering architecture.
 *
 * This is a temporary bridge during the migration process.
 */

#include "../rendering/Rendering.h"
#include "../math/Matrix.h"

namespace core {
    class Window;
}

namespace compat {

    /**
     * @brief Compatibility adapter for the old Renderer interface
     *
     * This class provides the same interface as the old core::Renderer but
     * internally uses the new rendering architecture. It allows existing code
     * to work unchanged while migrating to the new system.
     */
    class RendererAdapter {
    private:
        std::unique_ptr<rendering::twod::Renderer2D> renderer2D;
        core::Window* window;

    public:
        /**
         * @brief Constructor
         * @param window Window to render to
         */
        explicit RendererAdapter(core::Window& window);

        /**
         * @brief Destructor
         */
        ~RendererAdapter();

        // Prevent copy, allow move
        RendererAdapter(const RendererAdapter&) = delete;
        RendererAdapter& operator=(const RendererAdapter&) = delete;
        RendererAdapter(RendererAdapter&&) = default;
        RendererAdapter& operator=(RendererAdapter&&) = default;

        // ===== OLD RENDERER INTERFACE =====

        /**
         * @brief Clear the screen
         */
        void clear();

        /**
         * @brief Present the frame
         */
        void present();

        /**
         * @brief Set draw color
         */
        void setDrawColor(float r, float g, float b, float a);

        /**
         * @brief Set viewport
         */
        void setViewport(int x, int y, int width, int height);

        /**
         * @brief Get viewport size
         */
        void getViewportSize(int& width, int& height) const;

        /**
         * @brief Set projection matrix
         */
        void setProjectionMatrix(const math::Mat4& projection);

        /**
         * @brief Set view matrix
         */
        void setViewMatrix(const math::Mat4& view);

        /**
         * @brief Get projection matrix
         */
        const math::Mat4& getProjectionMatrix() const;

        /**
         * @brief Get view matrix
         */
        const math::Mat4& getViewMatrix() const;

        /**
         * @brief Get window reference
         */
        core::Window& getWindow() const { return *window; }

        // ===== NEW RENDERING INTERFACE =====

        /**
         * @brief Get the underlying 2D renderer for advanced usage
         */
        rendering::twod::Renderer2D* get2DRenderer() const { return renderer2D.get(); }

        /**
         * @brief Get the backend for advanced usage
         */
        rendering::backend::RenderBackend* getBackend() const;

        /**
         * @brief Draw a colored rectangle (new convenience method)
         */
        void drawRect(const rendering::twod::Rect2D& rect, const rendering::twod::Color& color);

        /**
         * @brief Draw a textured rectangle (new convenience method)
         */
        void drawTexturedRect(const rendering::twod::Rect2D& rect, rendering::backend::TextureHandle texture);

    private:
        // Current state for compatibility
        float currentR = 1.0f, currentG = 1.0f, currentB = 1.0f, currentA = 1.0f;
        math::Mat4 currentProjection;
        math::Mat4 currentView;
        bool projectionSet = false;
        bool viewSet = false;
    };

} // namespace compat

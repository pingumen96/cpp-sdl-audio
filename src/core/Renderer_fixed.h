#pragma once

// src/core/Renderer.h – New Stratified Rendering Architecture Integration
#include "../compat/RendererAdapter.h"
#include "../compat/IRenderBackendBridge.h"
#include "../rendering/Rendering.h"

#include <memory>

namespace core {
    class Window;

    /**
     * @brief Renderer class using the new stratified rendering architecture
     *
     * This class now uses the new 4-layer rendering architecture internally.
     * It maintains the same interface as before for backward compatibility
     * while providing access to the new powerful rendering system.
     */
    class Renderer {
    private:
        std::unique_ptr<compat::RendererAdapter> adapter;
        std::unique_ptr<compat::IRenderBackendBridge> legacyBridge;
        Window* window;

    public:
        /**
         * @brief Constructor
         * @param window Window to render to
         */
        explicit Renderer(Window& window);

        /**
         * @brief Destructor
         */
        ~Renderer();

        // Prevent copy, allow move
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;

        // ===== LEGACY INTERFACE (for backward compatibility) =====

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
        void setDrawColor(float r, float g, float b, float a = 1.0f);

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

        // ===== NEW ARCHITECTURE ACCESS =====

        /**
         * @brief Get the 2D renderer (new architecture)
         */
        rendering::twod::Renderer2D* get2DRenderer() const;

        /**
         * @brief Get the render backend (new architecture)
         */
        rendering::backend::RenderBackend* getBackend() const;

        /**
         * @brief Get the legacy render backend interface (for scene system)
         */
        scene::IRenderBackend* getRenderBackend() const;

        /**
         * @brief Draw a rectangle (new method)
         */
        void drawRect(const rendering::twod::Rect2D& rect, const rendering::twod::Color& color);

        /**
         * @brief Draw a textured rectangle (new method)
         */
        void drawTexturedRect(const rendering::twod::Rect2D& rect, rendering::backend::TextureHandle texture);
    };

} // namespace core

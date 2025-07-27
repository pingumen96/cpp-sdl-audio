#pragma once

// src/core/Renderer.h – OpenGL Renderer wrapper with scene system integration
#include "OpenGLBackend.h"
#include "../scene/rendering/IRenderBackend.h"

#include <memory>
#include <stdexcept>

namespace core {
    class Window;

    /**
     * @brief Renderer class that wraps OpenGLBackend and provides legacy compatibility
     *
     * This class now delegates to OpenGLBackend which implements IRenderBackend.
     * It provides the same interface as before for backward compatibility while
     * enabling integration with the scene system.
     */
    class Renderer {
    private:
        std::unique_ptr<OpenGLBackend> backend;
        Window* window; // Reference to the window for access

    public:
        Renderer(Window& window);

        // Basic rendering operations (delegated to backend)
        void clear();
        void present();
        void setDrawColor(float r, float g, float b, float a);

        // Viewport management
        void setViewport(int x, int y, int width, int height);
        void getViewportSize(int& width, int& height) const;

        // Matrix operations
        void setProjectionMatrix(const math::Mat4& projection);
        void setViewMatrix(const math::Mat4& view);
        const math::Mat4& getProjectionMatrix() const;
        const math::Mat4& getViewMatrix() const;

        // Access to the backend for scene system integration
        OpenGLBackend* getBackend() const { return backend.get(); }
        scene::IRenderBackend* getRenderBackend() const { return backend.get(); }

        // Access to the window
        Window& getWindow() const { return *window; }

        // Prevent copy
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        // Allow move
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;
    };
}
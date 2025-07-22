#pragma once

// src/core/Renderer.h – OpenGL Renderer wrapper
#include <GL/glew.h>
#include "../math/Matrix.h"

#include <memory>
#include <stdexcept>

namespace core {
    class Window;

    class Renderer {
    private:
        Window* window;
        math::Mat4 projectionMatrix;
        math::Mat4 viewMatrix;

        // Viewport dimensions
        int viewportWidth, viewportHeight;

    public:
        Renderer(Window& window);

        // Basic rendering operations
        void clear();
        void present();
        void setDrawColor(float r, float g, float b, float a);

        // Viewport management
        void setViewport(int x, int y, int width, int height);
        void getViewportSize(int& width, int& height) const;

        // Matrix operations
        void setProjectionMatrix(const math::Mat4& projection);
        void setViewMatrix(const math::Mat4& view);
        const math::Mat4& getProjectionMatrix() const { return projectionMatrix; }
        const math::Mat4& getViewMatrix() const { return viewMatrix; }

        // Prevent copy
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        // Allow move
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;
    };
}
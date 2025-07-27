#pragma once

#include "../scene/rendering/IRenderBackend.h"
#include "../math/Matrix.h"
#include <GL/glew.h>
#include <memory>
#include <string>

namespace core {
    class Window;

    /**
     * @brief OpenGL implementation of IRenderBackend
     *
     * This backend uses OpenGL for rendering and integrates with the existing
     * Window/Renderer infrastructure while implementing the scene system interface.
     */
    class OpenGLBackend : public scene::IRenderBackend {
    private:
        Window* window;
        bool initialized = false;
        uint32_t width = 0;
        uint32_t height = 0;

        // Matrices for compatibility with existing renderer
        math::Mat4 projectionMatrix;
        math::Mat4 viewMatrix;

        // OpenGL resources for quad rendering
        unsigned int quadVAO = 0;
        unsigned int quadVBO = 0;
        unsigned int simpleShaderProgram = 0;

        // Private methods
        void initializeQuadRendering();
        void renderUnitQuad(const scene::DrawItem& item);
        void cleanupQuadRendering();

    public:
        /**
         * @brief Constructor
         * @param window Window to render to
         */
        explicit OpenGLBackend(Window& window);

        /**
         * @brief Destructor
         */
        ~OpenGLBackend() override;

        // IRenderBackend implementation
        bool init(uint32_t width, uint32_t height) override;
        bool beginFrame() override;
        bool submit(const scene::CommandBuffer& commandBuffer) override;
        bool present() override;
        void shutdown() override;
        bool resize(uint32_t width, uint32_t height) override;
        std::string getBackendType() const override;
        bool isInitialized() const override;

        // Additional methods for compatibility with existing renderer interface
        void clear();
        void setDrawColor(float r, float g, float b, float a);
        void setViewport(int x, int y, int width, int height);
        void getViewportSize(int& width, int& height) const;
        void setProjectionMatrix(const math::Mat4& projection);
        void setViewMatrix(const math::Mat4& view);
        const math::Mat4& getProjectionMatrix() const { return projectionMatrix; }
        const math::Mat4& getViewMatrix() const { return viewMatrix; }

        // Prevent copy
        OpenGLBackend(const OpenGLBackend&) = delete;
        OpenGLBackend& operator=(const OpenGLBackend&) = delete;

        // Allow move
        OpenGLBackend(OpenGLBackend&&) = default;
        OpenGLBackend& operator=(OpenGLBackend&&) = default;
    };
}

#pragma once

#include "../RenderDevice.h"
#include <GL/glew.h>
#include <memory>

namespace core {
    class Window;
}

namespace rendering::hal {

    /**
     * @brief OpenGL implementation of RenderDevice
     *
     * This class implements the hardware abstraction layer using OpenGL API.
     * It manages OpenGL state, resources and provides immediate mode rendering.
     */
    class OpenGLDevice : public RenderDevice {
    private:
        core::Window* window;
        bool initialized = false;

        // Current viewport
        int viewportX = 0, viewportY = 0;
        int viewportWidth = 0, viewportHeight = 0;

        // Current matrices
        math::Mat4 projectionMatrix;
        math::Mat4 viewMatrix;

        // Quad rendering resources
        GLuint quadVAO = 0;
        GLuint quadVBO = 0;
        GLuint simpleColorProgram = 0;
        GLuint simpleTextureProgram = 0;

        // Shader uniform locations
        GLint colorProgram_mvpLocation = -1;
        GLint colorProgram_colorLocation = -1;
        GLint textureProgram_mvpLocation = -1;
        GLint textureProgram_textureLocation = -1;

        // Private methods
        void initializeQuadResources();
        void cleanupQuadResources();
        bool compileShader(GLuint shader, const std::string& source);
        bool linkProgram(GLuint program);
        math::Mat4 calculateMVP(const math::Mat4& modelMatrix) const;

    public:
        /**
         * @brief Constructor
         * @param window Window context for OpenGL
         */
        explicit OpenGLDevice(core::Window& window);

        /**
         * @brief Destructor
         */
        ~OpenGLDevice() override;

        // ===== LIFECYCLE =====
        bool initialize(uint32_t width, uint32_t height) override;
        void shutdown() override;

        // ===== FRAME MANAGEMENT =====
        void beginFrame() override;
        void endFrame() override;
        void clear(float r, float g, float b, float a) override;

        // ===== VIEWPORT AND MATRICES =====
        void setViewport(int x, int y, int width, int height) override;
        void getViewportSize(int& width, int& height) const override;
        void setProjectionMatrix(const math::Mat4& projection) override;
        void setViewMatrix(const math::Mat4& view) override;

        // ===== IMMEDIATE MODE RENDERING =====
        void drawQuad(const math::Mat4& modelMatrix,
            float r, float g, float b, float a) override;
        void drawTexturedQuad(const math::Mat4& modelMatrix,
            uint32_t textureId) override;

        // ===== RESOURCE MANAGEMENT =====
        uint32_t createTexture(const unsigned char* data,
            int width, int height, int channels) override;
        void deleteTexture(uint32_t textureId) override;
        uint32_t createShaderProgram(const std::string& vertexSource,
            const std::string& fragmentSource) override;
        void deleteShaderProgram(uint32_t programId) override;

        // ===== DEVICE INFO =====
        std::string getDeviceName() const override;
        bool supportsFeature(const std::string& feature) const override;

        // ===== OPENGL SPECIFIC =====

        /**
         * @brief Get OpenGL version string
         */
        std::string getOpenGLVersion() const;

        /**
         * @brief Get current OpenGL error (for debugging)
         */
        GLenum getLastError() const { return glGetError(); }
    };

} // namespace rendering::hal

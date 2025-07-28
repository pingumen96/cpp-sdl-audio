#pragma once

#include "../../math/Matrix.h"
#include <cstdint>
#include <string>
#include <memory>

namespace rendering::hal {

    /**
     * @brief Hardware abstraction layer - abstract interface for graphics devices
     *
     * This interface provides the lowest level abstraction over graphics APIs
     * like OpenGL, Vulkan, DirectX. It deals with hardware resources and
     * immediate mode rendering commands.
     */
    class RenderDevice {
    public:
        virtual ~RenderDevice() = default;

        // ===== LIFECYCLE =====

        /**
         * @brief Initialize the render device
         * @param width Initial framebuffer width
         * @param height Initial framebuffer height
         * @return true if initialization succeeded
         */
        virtual bool initialize(uint32_t width, uint32_t height) = 0;

        /**
         * @brief Shutdown the render device and release resources
         */
        virtual void shutdown() = 0;

        // ===== FRAME MANAGEMENT =====

        /**
         * @brief Begin a new frame
         */
        virtual void beginFrame() = 0;

        /**
         * @brief End the current frame and present
         */
        virtual void endFrame() = 0;

        /**
         * @brief Clear the framebuffer
         * @param r Red component [0,1]
         * @param g Green component [0,1]
         * @param b Blue component [0,1]
         * @param a Alpha component [0,1]
         */
        virtual void clear(float r, float g, float b, float a) = 0;

        // ===== VIEWPORT AND MATRICES =====

        /**
         * @brief Set viewport dimensions
         */
        virtual void setViewport(int x, int y, int width, int height) = 0;

        /**
         * @brief Get current viewport size
         */
        virtual void getViewportSize(int& width, int& height) const = 0;

        /**
         * @brief Set projection matrix
         */
        virtual void setProjectionMatrix(const math::Mat4& projection) = 0;

        /**
         * @brief Set view matrix
         */
        virtual void setViewMatrix(const math::Mat4& view) = 0;

        // ===== IMMEDIATE MODE RENDERING =====

        /**
         * @brief Render a colored quad at specified transform
         * @param modelMatrix Transform matrix
         * @param r Red component [0,1]
         * @param g Green component [0,1]
         * @param b Blue component [0,1]
         * @param a Alpha component [0,1]
         */
        virtual void drawQuad(const math::Mat4& modelMatrix,
            float r, float g, float b, float a) = 0;

        /**
         * @brief Render a textured quad at specified transform
         * @param modelMatrix Transform matrix
         * @param textureId Texture identifier
         */
        virtual void drawTexturedQuad(const math::Mat4& modelMatrix,
            uint32_t textureId) = 0;

        // ===== RESOURCE MANAGEMENT =====

        /**
         * @brief Load texture from memory
         * @param data Pixel data
         * @param width Texture width
         * @param height Texture height
         * @param channels Number of channels (3=RGB, 4=RGBA)
         * @return Texture ID or 0 if failed
         */
        virtual uint32_t createTexture(const unsigned char* data,
            int width, int height, int channels) = 0;

        /**
         * @brief Delete a texture
         */
        virtual void deleteTexture(uint32_t textureId) = 0;

        /**
         * @brief Load shader program from source
         * @param vertexSource Vertex shader source code
         * @param fragmentSource Fragment shader source code
         * @return Shader program ID or 0 if failed
         */
        virtual uint32_t createShaderProgram(const std::string& vertexSource,
            const std::string& fragmentSource) = 0;

        /**
         * @brief Delete a shader program
         */
        virtual void deleteShaderProgram(uint32_t programId) = 0;

        // ===== DEVICE INFO =====

        /**
         * @brief Get device name/description
         */
        virtual std::string getDeviceName() const = 0;

        /**
         * @brief Check if device supports specific feature
         */
        virtual bool supportsFeature(const std::string& feature) const = 0;
    };

    /**
     * @brief Factory function to create platform-specific render device
     */
    std::unique_ptr<RenderDevice> createDefaultRenderDevice();

} // namespace rendering::hal

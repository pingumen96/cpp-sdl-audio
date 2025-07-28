#pragma once

#include "../hal/RenderDevice.h"
#include "CommandBuffer.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace rendering::backend {

    /**
     * @brief Resource handle for textures
     */
    using TextureHandle = uint32_t;

    /**
     * @brief Resource handle for shaders
     */
    using ShaderHandle = uint32_t;

    /**
     * @brief Texture creation parameters
     */
    struct TextureCreateInfo {
        const unsigned char* data;
        int width;
        int height;
        int channels;
        std::string name; // For debugging/identification

        TextureCreateInfo(const unsigned char* d, int w, int h, int c, const std::string& n = "")
            : data(d), width(w), height(h), channels(c), name(n) {}
    };

    /**
     * @brief Shader creation parameters
     */
    struct ShaderCreateInfo {
        std::string vertexSource;
        std::string fragmentSource;
        std::string name; // For debugging/identification

        ShaderCreateInfo(const std::string& vs, const std::string& fs, const std::string& n = "")
            : vertexSource(vs), fragmentSource(fs), name(n) {}
    };

    /**
     * @brief Render backend that orchestrates the HAL and manages resources
     *
     * This class sits between the HAL (hardware abstraction) and the 2D frontend.
     * It manages resources, executes command buffers, and provides a clean API
     * that's independent of the underlying graphics API.
     */
    class RenderBackend {
    private:
        std::unique_ptr<hal::RenderDevice> device;
        bool initialized = false;

        // Resource tracking
        std::unordered_map<TextureHandle, std::string> textureNames;
        std::unordered_map<ShaderHandle, std::string> shaderNames;
        TextureHandle nextTextureHandle = 1;
        ShaderHandle nextShaderHandle = 1;

        // Current render state
        int currentViewportX = 0, currentViewportY = 0;
        int currentViewportWidth = 0, currentViewportHeight = 0;

    public:
        /**
         * @brief Constructor
         * @param renderDevice HAL device to use for rendering
         */
        explicit RenderBackend(std::unique_ptr<hal::RenderDevice> renderDevice);

        /**
         * @brief Destructor
         */
        ~RenderBackend();

        // Prevent copy, allow move
        RenderBackend(const RenderBackend&) = delete;
        RenderBackend& operator=(const RenderBackend&) = delete;
        RenderBackend(RenderBackend&&) = default;
        RenderBackend& operator=(RenderBackend&&) = default;

        // ===== LIFECYCLE =====

        /**
         * @brief Initialize the render backend
         * @param width Initial framebuffer width
         * @param height Initial framebuffer height
         * @return true if initialization succeeded
         */
        bool initialize(uint32_t width, uint32_t height);

        /**
         * @brief Shutdown the render backend
         */
        void shutdown();

        /**
         * @brief Check if backend is initialized
         */
        bool isInitialized() const { return initialized; }

        // ===== FRAME MANAGEMENT =====

        /**
         * @brief Begin a new frame
         */
        void beginFrame();

        /**
         * @brief Execute a command buffer
         * @param commandBuffer Commands to execute
         */
        void executeCommandBuffer(const CommandBuffer& commandBuffer);

        /**
         * @brief End the current frame and present
         */
        void endFrame();

        // ===== RESOURCE MANAGEMENT =====

        /**
         * @brief Create a texture resource
         * @param createInfo Texture creation parameters
         * @return Texture handle or 0 if failed
         */
        TextureHandle createTexture(const TextureCreateInfo& createInfo);

        /**
         * @brief Delete a texture resource
         * @param handle Texture handle to delete
         */
        void deleteTexture(TextureHandle handle);

        /**
         * @brief Create a shader program
         * @param createInfo Shader creation parameters
         * @return Shader handle or 0 if failed
         */
        ShaderHandle createShader(const ShaderCreateInfo& createInfo);

        /**
         * @brief Delete a shader program
         * @param handle Shader handle to delete
         */
        void deleteShader(ShaderHandle handle);

        // ===== IMMEDIATE MODE API (for simple cases) =====

        /**
         * @brief Clear the framebuffer
         */
        void clear(float r, float g, float b, float a);

        /**
         * @brief Set viewport
         */
        void setViewport(int x, int y, int width, int height);

        /**
         * @brief Set projection matrix
         */
        void setProjectionMatrix(const math::Mat4& projection);

        /**
         * @brief Set view matrix
         */
        void setViewMatrix(const math::Mat4& view);

        /**
         * @brief Draw a colored quad immediately
         */
        void drawQuad(const math::Mat4& modelMatrix, float r, float g, float b, float a);

        /**
         * @brief Draw a textured quad immediately
         */
        void drawTexturedQuad(const math::Mat4& modelMatrix, TextureHandle texture);

        // ===== INFORMATION =====

        /**
         * @brief Get device information
         */
        std::string getDeviceInfo() const;

        /**
         * @brief Get current viewport dimensions
         */
        void getViewportSize(int& width, int& height) const;

        /**
         * @brief Check if feature is supported
         */
        bool supportsFeature(const std::string& feature) const;

        /**
         * @brief Get resource usage statistics
         */
        struct ResourceStats {
            size_t textureCount;
            size_t shaderCount;
        };
        ResourceStats getResourceStats() const;

        // ===== TESTING/DEBUG =====

        /**
         * @brief Get the underlying HAL device (for testing)
         */
        hal::RenderDevice* getHALDevice() const { return device.get(); }

    private:
        void executeCommand(const DrawCommand& command);
    };

} // namespace rendering::backend

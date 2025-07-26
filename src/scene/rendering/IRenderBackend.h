#pragma once

#include "CommandBuffer.h"
#include <memory>

namespace scene {

    /**
     * @brief Abstract interface for render backends
     *
     * This interface provides a device-level abstraction for rendering.
     * Implementations can use OpenGL, Vulkan, DirectX, or be null for testing.
     */
    class IRenderBackend {
    public:
        virtual ~IRenderBackend() = default;

        /**
         * @brief Initialize the render backend
         * @param width Initial render target width
         * @param height Initial render target height
         * @return true if initialization succeeded
         */
        virtual bool init(uint32_t width, uint32_t height) = 0;

        /**
         * @brief Begin a new frame
         * @return true if frame started successfully
         */
        virtual bool beginFrame() = 0;

        /**
         * @brief Submit a command buffer for rendering
         * @param commandBuffer Commands to execute
         * @return true if submission succeeded
         */
        virtual bool submit(const CommandBuffer& commandBuffer) = 0;

        /**
         * @brief Present the current frame
         * @return true if presentation succeeded
         */
        virtual bool present() = 0;

        /**
         * @brief Shutdown the render backend
         */
        virtual void shutdown() = 0;

        /**
         * @brief Resize the render target
         * @param width New width
         * @param height New height
         * @return true if resize succeeded
         */
        virtual bool resize(uint32_t width, uint32_t height) = 0;

        /**
         * @brief Get backend type name for debugging
         */
        virtual std::string getBackendType() const = 0;

        /**
         * @brief Check if backend is initialized
         */
        virtual bool isInitialized() const = 0;
    };

    using RenderBackendPtr = std::unique_ptr<IRenderBackend>;

} // namespace scene

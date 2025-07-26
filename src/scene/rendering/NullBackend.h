#pragma once

#include "IRenderBackend.h"
#include <iostream>
#include <sstream>

namespace scene {

    /**
     * @brief Null render backend for testing and development
     *
     * This backend logs rendering commands without actually rendering anything.
     * Useful for testing scene system logic without requiring a graphics context.
     */
    class NullBackend : public IRenderBackend {
    private:
        bool initialized = false;
        uint32_t width = 0;
        uint32_t height = 0;
        size_t frameCount = 0;

    public:
        /**
         * @brief Initialize the null backend
         */
        bool init(uint32_t width, uint32_t height) override {
            this->width = width;
            this->height = height;
            this->initialized = true;

            std::cout << "[NullBackend] Initialized with size: "
                << width << "x" << height << std::endl;
            return true;
        }

        /**
         * @brief Begin a new frame
         */
        bool beginFrame() override {
            if (!initialized) {
                std::cerr << "[NullBackend] Error: Not initialized!" << std::endl;
                return false;
            }

            frameCount++;
            std::cout << "[NullBackend] Begin frame #" << frameCount << std::endl;
            return true;
        }

        /**
         * @brief Submit commands (log them)
         */
        bool submit(const CommandBuffer& commandBuffer) override {
            if (!initialized) {
                std::cerr << "[NullBackend] Error: Not initialized!" << std::endl;
                return false;
            }

            std::cout << "[NullBackend] Submitting command buffer:" << std::endl;

            // Log draw items
            const auto& drawItems = commandBuffer.getDrawItems();
            std::cout << "  - Draw items: " << drawItems.size() << std::endl;
            for (size_t i = 0; i < drawItems.size(); ++i) {
                const auto& item = drawItems[i];
                std::cout << "    [" << i << "] Mesh: " << item.meshId
                    << ", Material: " << item.materialId
                    << ", Layer: " << item.renderLayer << std::endl;
            }

            // Log post effects
            const auto& postEffects = commandBuffer.getPostEffects();
            std::cout << "  - Post effects: " << postEffects.size() << std::endl;
            for (size_t i = 0; i < postEffects.size(); ++i) {
                const auto& effect = postEffects[i];
                std::cout << "    [" << i << "] Effect: " << effect.effectId
                    << ", Enabled: " << (effect.enabled ? "Yes" : "No") << std::endl;
            }

            // Log UI items
            const auto& uiItems = commandBuffer.getUIItems();
            std::cout << "  - UI items: " << uiItems.size() << std::endl;
            for (size_t i = 0; i < uiItems.size(); ++i) {
                const auto& item = uiItems[i];
                std::cout << "    [" << i << "] Texture: " << item.textureId
                    << ", Pos: (" << item.position.x << ", " << item.position.y << ")"
                    << ", Layer: " << item.layer << std::endl;
            }

            // Log camera info
            const auto& camera = commandBuffer.getCamera();
            std::cout << "  - Camera pos: (" << camera.position.x
                << ", " << camera.position.y
                << ", " << camera.position.z << ")" << std::endl;

            // Log render target
            const auto& target = commandBuffer.getRenderTarget();
            std::cout << "  - Target: " << target.targetId
                << " (" << target.width << "x" << target.height << ")" << std::endl;

            return true;
        }

        /**
         * @brief Present frame (log)
         */
        bool present() override {
            if (!initialized) {
                std::cerr << "[NullBackend] Error: Not initialized!" << std::endl;
                return false;
            }

            std::cout << "[NullBackend] Present frame #" << frameCount << std::endl;
            return true;
        }

        /**
         * @brief Shutdown backend
         */
        void shutdown() override {
            if (initialized) {
                std::cout << "[NullBackend] Shutdown after " << frameCount
                    << " frames" << std::endl;
                initialized = false;
                frameCount = 0;
            }
        }

        /**
         * @brief Resize render target
         */
        bool resize(uint32_t newWidth, uint32_t newHeight) override {
            if (!initialized) {
                std::cerr << "[NullBackend] Error: Not initialized!" << std::endl;
                return false;
            }

            std::cout << "[NullBackend] Resize from " << width << "x" << height
                << " to " << newWidth << "x" << newHeight << std::endl;

            this->width = newWidth;
            this->height = newHeight;
            return true;
        }

        /**
         * @brief Get backend type name
         */
        std::string getBackendType() const override {
            return "NullBackend";
        }

        /**
         * @brief Check if initialized
         */
        bool isInitialized() const override {
            return initialized;
        }

        /**
         * @brief Get current frame count (for testing)
         */
        size_t getFrameCount() const {
            return frameCount;
        }

        /**
         * @brief Get current render target size
         */
        std::pair<uint32_t, uint32_t> getSize() const {
            return { width, height };
        }
    };

} // namespace scene

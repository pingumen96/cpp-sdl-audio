#pragma once

#include "../RenderDevice.h"
#include <vector>
#include <unordered_map>

namespace rendering::hal {

    /**
     * @brief Null implementation of RenderDevice for testing
     *
     * This device doesn't actually render anything but tracks all operations
     * for validation in unit tests. Useful for headless testing and debugging.
     */
    class NullDevice : public RenderDevice {
    private:
        bool initialized = false;
        int viewportWidth = 0;
        int viewportHeight = 0;
        math::Mat4 projectionMatrix;
        math::Mat4 viewMatrix;

        // Track operations for testing
        mutable std::vector<std::string> operations;
        std::unordered_map<uint32_t, std::string> textures;
        std::unordered_map<uint32_t, std::string> shaderPrograms;
        uint32_t nextTextureId = 1;
        uint32_t nextShaderId = 1;

    public:
        NullDevice() = default;
        ~NullDevice() override = default;

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

        // ===== TESTING UTILITIES =====

        /**
         * @brief Get all recorded operations for testing
         */
        const std::vector<std::string>& getOperations() const { return operations; }

        /**
         * @brief Clear operation history
         */
        void clearOperations() { operations.clear(); }

        /**
         * @brief Check if specific operation was recorded
         */
        bool hasOperation(const std::string& operation) const;

        /**
         * @brief Get number of operations of specific type
         */
        size_t countOperations(const std::string& prefix) const;

        /**
         * @brief Get current matrices for testing
         */
        const math::Mat4& getProjectionMatrix() const { return projectionMatrix; }
        const math::Mat4& getViewMatrix() const { return viewMatrix; }

    private:
        void recordOperation(const std::string& operation) const;
    };

} // namespace rendering::hal

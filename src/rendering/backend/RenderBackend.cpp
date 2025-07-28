#include "RenderBackend.h"
#include <iostream>

namespace rendering::backend {

    RenderBackend::RenderBackend(std::unique_ptr<hal::RenderDevice> renderDevice)
        : device(std::move(renderDevice)) {}

    RenderBackend::~RenderBackend() {
        if (initialized) {
            shutdown();
        }
    }

    bool RenderBackend::initialize(uint32_t width, uint32_t height) {
        if (initialized) {
            return true;
        }

        if (!device) {
            std::cerr << "[RenderBackend] No HAL device provided" << std::endl;
            return false;
        }

        if (!device->initialize(width, height)) {
            std::cerr << "[RenderBackend] Failed to initialize HAL device" << std::endl;
            return false;
        }

        currentViewportWidth = static_cast<int>(width);
        currentViewportHeight = static_cast<int>(height);

        std::cout << "[RenderBackend] Initialized with device: " << device->getDeviceName() << std::endl;
        initialized = true;
        return true;
    }

    void RenderBackend::shutdown() {
        if (!initialized) {
            return;
        }

        std::cout << "[RenderBackend] Shutting down..." << std::endl;

        // Clean up all resources
        for (const auto& pair : textureNames) {
            device->deleteTexture(pair.first);
        }
        textureNames.clear();

        for (const auto& pair : shaderNames) {
            device->deleteShaderProgram(pair.first);
        }
        shaderNames.clear();

        // Shutdown HAL device
        device->shutdown();
        initialized = false;
    }

    void RenderBackend::beginFrame() {
        if (!initialized) return;
        device->beginFrame();
    }

    void RenderBackend::executeCommandBuffer(const CommandBuffer& commandBuffer) {
        if (!initialized) return;

        for (const auto& command : commandBuffer.getCommands()) {
            executeCommand(*command);
        }
    }

    void RenderBackend::endFrame() {
        if (!initialized) return;
        device->endFrame();
    }

    TextureHandle RenderBackend::createTexture(const TextureCreateInfo& createInfo) {
        if (!initialized) return 0;

        uint32_t halTextureId = device->createTexture(createInfo.data, createInfo.width,
            createInfo.height, createInfo.channels);
        if (halTextureId == 0) {
            std::cerr << "[RenderBackend] Failed to create texture: " << createInfo.name << std::endl;
            return 0;
        }

        TextureHandle handle = nextTextureHandle++;
        textureNames[handle] = createInfo.name;

        // For now, we use the HAL texture ID directly as handle
        // In a more complex system, we'd maintain a mapping
        return halTextureId;
    }

    void RenderBackend::deleteTexture(TextureHandle handle) {
        if (!initialized) return;

        device->deleteTexture(handle);
        textureNames.erase(handle);
    }

    ShaderHandle RenderBackend::createShader(const ShaderCreateInfo& createInfo) {
        if (!initialized) return 0;

        uint32_t halShaderId = device->createShaderProgram(createInfo.vertexSource,
            createInfo.fragmentSource);
        if (halShaderId == 0) {
            std::cerr << "[RenderBackend] Failed to create shader: " << createInfo.name << std::endl;
            return 0;
        }

        ShaderHandle handle = nextShaderHandle++;
        shaderNames[handle] = createInfo.name;

        // For now, we use the HAL shader ID directly as handle
        return halShaderId;
    }

    void RenderBackend::deleteShader(ShaderHandle handle) {
        if (!initialized) return;

        device->deleteShaderProgram(handle);
        shaderNames.erase(handle);
    }

    void RenderBackend::clear(float r, float g, float b, float a) {
        if (!initialized) return;
        device->clear(r, g, b, a);
    }

    void RenderBackend::setViewport(int x, int y, int width, int height) {
        if (!initialized) return;

        device->setViewport(x, y, width, height);
        currentViewportX = x;
        currentViewportY = y;
        currentViewportWidth = width;
        currentViewportHeight = height;
    }

    void RenderBackend::setProjectionMatrix(const math::Mat4& projection) {
        if (!initialized) return;
        device->setProjectionMatrix(projection);
    }

    void RenderBackend::setViewMatrix(const math::Mat4& view) {
        if (!initialized) return;
        device->setViewMatrix(view);
    }

    void RenderBackend::drawQuad(const math::Mat4& modelMatrix, float r, float g, float b, float a) {
        if (!initialized) return;
        device->drawQuad(modelMatrix, r, g, b, a);
    }

    void RenderBackend::drawTexturedQuad(const math::Mat4& modelMatrix, TextureHandle texture) {
        if (!initialized) return;
        device->drawTexturedQuad(modelMatrix, texture);
    }

    std::string RenderBackend::getDeviceInfo() const {
        if (!device) return "No device";
        return device->getDeviceName();
    }

    void RenderBackend::getViewportSize(int& width, int& height) const {
        width = currentViewportWidth;
        height = currentViewportHeight;
    }

    bool RenderBackend::supportsFeature(const std::string& feature) const {
        if (!device) return false;
        return device->supportsFeature(feature);
    }

    RenderBackend::ResourceStats RenderBackend::getResourceStats() const {
        return ResourceStats{
            .textureCount = textureNames.size(),
            .shaderCount = shaderNames.size()
        };
    }

    void RenderBackend::executeCommand(const DrawCommand& command) {
        switch (command.type) {
        case DrawCommandType::CLEAR: {
            const auto& clearCmd = static_cast<const ClearCommand&>(command);
            device->clear(clearCmd.r, clearCmd.g, clearCmd.b, clearCmd.a);
            break;
        }

        case DrawCommandType::DRAW_QUAD: {
            const auto& quadCmd = static_cast<const DrawQuadCommand&>(command);
            device->drawQuad(quadCmd.modelMatrix, quadCmd.r, quadCmd.g, quadCmd.b, quadCmd.a);
            break;
        }

        case DrawCommandType::DRAW_TEXTURED_QUAD: {
            const auto& texQuadCmd = static_cast<const DrawTexturedQuadCommand&>(command);
            device->drawTexturedQuad(texQuadCmd.modelMatrix, texQuadCmd.textureId);
            break;
        }

        case DrawCommandType::SET_VIEWPORT: {
            const auto& viewportCmd = static_cast<const ViewportCommand&>(command);
            device->setViewport(viewportCmd.x, viewportCmd.y, viewportCmd.width, viewportCmd.height);
            currentViewportX = viewportCmd.x;
            currentViewportY = viewportCmd.y;
            currentViewportWidth = viewportCmd.width;
            currentViewportHeight = viewportCmd.height;
            break;
        }

        case DrawCommandType::SET_PROJECTION_MATRIX: {
            const auto& projCmd = static_cast<const ProjectionMatrixCommand&>(command);
            device->setProjectionMatrix(projCmd.matrix);
            break;
        }

        case DrawCommandType::SET_VIEW_MATRIX: {
            const auto& viewCmd = static_cast<const ViewMatrixCommand&>(command);
            device->setViewMatrix(viewCmd.matrix);
            break;
        }
        }
    }

} // namespace rendering::backend

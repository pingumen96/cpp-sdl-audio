#include "Renderer2D.h"
#include <algorithm>
#include <iostream>

namespace rendering::twod {

    Renderer2D::Renderer2D(std::unique_ptr<backend::RenderBackend> renderBackend,
        const Renderer2DConfig& config)
        : backend(std::move(renderBackend)), config(config) {}

    Renderer2D::~Renderer2D() {
        if (backend && backend->isInitialized()) {
            shutdown();
        }
    }

    bool Renderer2D::initialize(uint32_t width, uint32_t height) {
        if (!backend) {
            std::cerr << "[Renderer2D] No backend provided" << std::endl;
            return false;
        }

        if (!backend->initialize(width, height)) {
            std::cerr << "[Renderer2D] Failed to initialize backend" << std::endl;
            return false;
        }

        // Set initial viewport
        setViewportSize(static_cast<float>(width), static_cast<float>(height));

        std::cout << "[Renderer2D] Initialized 2D renderer" << std::endl;
        std::cout << "[Renderer2D] Backend: " << backend->getDeviceInfo() << std::endl;
        std::cout << "[Renderer2D] Max quads per batch: " << config.maxQuadsPerBatch << std::endl;

        return true;
    }

    void Renderer2D::shutdown() {
        if (backend) {
            backend->shutdown();
        }
    }

    void Renderer2D::beginFrame() {
        if (!backend) return;

        backend->beginFrame();
        commandBuffer.clear();

        // Update matrices if needed
        if (projectionDirty) {
            updateProjectionMatrix();
            projectionDirty = false;
        }

        if (viewDirty) {
            updateViewMatrix();
            viewDirty = false;
        }
    }

    void Renderer2D::endFrame() {
        if (!backend) return;

        // Flush any remaining batch
        if (batchStarted) {
            endBatch();
        }

        // Execute accumulated commands
        if (!commandBuffer.empty()) {
            backend->executeCommandBuffer(commandBuffer);
        }

        backend->endFrame();
    }

    void Renderer2D::setCameraPosition(float x, float y) {
        if (cameraX != x || cameraY != y) {
            cameraX = x;
            cameraY = y;
            viewDirty = true;
        }
    }

    void Renderer2D::getCameraPosition(float& x, float& y) const {
        x = cameraX;
        y = cameraY;
    }

    void Renderer2D::setCameraZoom(float newZoom) {
        if (newZoom != zoom && newZoom > 0.0f) {
            zoom = newZoom;
            projectionDirty = true;
        }
    }

    void Renderer2D::setViewportSize(float width, float height) {
        if (viewportWidth != width || viewportHeight != height) {
            viewportWidth = width;
            viewportHeight = height;
            projectionDirty = true;

            if (backend) {
                backend->setViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
            }
        }
    }

    void Renderer2D::getViewportSize(float& width, float& height) const {
        width = viewportWidth;
        height = viewportHeight;
    }

    void Renderer2D::clear(const Color& color) {
        if (!backend) return;
        commandBuffer.clear(color.r, color.g, color.b, color.a);
    }

    void Renderer2D::drawRect(const Rect2D& rect, const Color& color) {
        if (!backend) return;

        math::Mat4 transform = createTransformMatrix(rect);
        commandBuffer.drawQuad(transform, color.r, color.g, color.b, color.a);
    }

    void Renderer2D::drawTexturedRect(const Rect2D& rect, backend::TextureHandle texture) {
        if (!backend) return;

        math::Mat4 transform = createTransformMatrix(rect);
        commandBuffer.drawTexturedQuad(transform, texture);
    }

    void Renderer2D::drawQuad(const math::Mat4& transform, const Color& color) {
        if (!backend) return;
        commandBuffer.drawQuad(transform, color.r, color.g, color.b, color.a);
    }

    void Renderer2D::drawTexturedQuad(const math::Mat4& transform, backend::TextureHandle texture) {
        if (!backend) return;
        commandBuffer.drawTexturedQuad(transform, texture);
    }

    void Renderer2D::beginBatch() {
        if (batchStarted) {
            std::cerr << "[Renderer2D] Warning: beginBatch() called while batch already started" << std::endl;
            return;
        }

        currentBatch.clear();
        batchStarted = true;
    }

    void Renderer2D::batchRect(const Rect2D& rect, const Color& color, float depth) {
        if (!batchStarted) {
            std::cerr << "[Renderer2D] Warning: batchRect() called without beginBatch()" << std::endl;
            return;
        }

        currentBatch.emplace_back(rect, color, depth);

        // Auto-flush if batch is full
        if (currentBatch.size() >= config.maxQuadsPerBatch) {
            flushBatch();
        }
    }

    void Renderer2D::batchTexturedRect(const Rect2D& rect, backend::TextureHandle texture, float depth) {
        if (!batchStarted) {
            std::cerr << "[Renderer2D] Warning: batchTexturedRect() called without beginBatch()" << std::endl;
            return;
        }

        currentBatch.emplace_back(rect, texture, depth);

        // Auto-flush if batch is full
        if (currentBatch.size() >= config.maxQuadsPerBatch) {
            flushBatch();
        }
    }

    void Renderer2D::endBatch() {
        if (!batchStarted) {
            std::cerr << "[Renderer2D] Warning: endBatch() called without beginBatch()" << std::endl;
            return;
        }

        flushBatch();
        batchStarted = false;
    }

    backend::TextureHandle Renderer2D::createTexture(const unsigned char* data, int width, int height,
        int channels, const std::string& name) {
        if (!backend) return 0;

        backend::TextureCreateInfo createInfo(data, width, height, channels, name);
        return backend->createTexture(createInfo);
    }

    void Renderer2D::deleteTexture(backend::TextureHandle texture) {
        if (!backend) return;
        backend->deleteTexture(texture);
    }

    std::string Renderer2D::getRendererInfo() const {
        if (!backend) return "No backend";

        std::string info = "Renderer2D using ";
        info += backend->getDeviceInfo();

        auto stats = backend->getResourceStats();
        info += " | Textures: " + std::to_string(stats.textureCount);
        info += " | Shaders: " + std::to_string(stats.shaderCount);

        return info;
    }

    // Private methods

    void Renderer2D::updateProjectionMatrix() {
        // Create orthographic projection matrix
        float halfWidth = (viewportWidth / 2.0f) / zoom;
        float halfHeight = (viewportHeight / 2.0f) / zoom;

        projectionMatrix = math::Mat4::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
            config.nearPlane, config.farPlane);

        if (backend) {
            commandBuffer.setProjectionMatrix(projectionMatrix);
        }
    }

    void Renderer2D::updateViewMatrix() {
        // Create view matrix (inverse of camera transform)
        viewMatrix = math::Mat4::translation(-cameraX, -cameraY, 0.0f);

        if (backend) {
            commandBuffer.setViewMatrix(viewMatrix);
        }
    }    void Renderer2D::flushBatch() {
        if (currentBatch.empty()) {
            return;
        }

        // Sort by depth if enabled
        if (config.enableDepthSorting) {
            sortBatchByDepth();
        }

        // Render all items in the batch
        for (const auto& item : currentBatch) {
            math::Mat4 transform = createTransformMatrix(item.rect);

            if (item.hasTexture) {
                commandBuffer.drawTexturedQuad(transform, item.texture);
            } else {
                commandBuffer.drawQuad(transform, item.color.r, item.color.g, item.color.b, item.color.a);
            }
        }

        currentBatch.clear();
    }

    math::Mat4 Renderer2D::createTransformMatrix(const Rect2D& rect) const {
        return math::Mat4::translation(rect.position.x(), rect.position.y(), 0.0f) *
            math::Mat4::scale(rect.size.x(), rect.size.y(), 1.0f);
    }    void Renderer2D::sortBatchByDepth() {
        std::sort(currentBatch.begin(), currentBatch.end(),
            [](const DrawItem2D& a, const DrawItem2D& b) {
                return a.depth < b.depth; // Back to front
            });
    }

} // namespace rendering::twod

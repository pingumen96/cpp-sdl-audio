#include "OpenGLBackend.h"
#include "Window.h"
#include "../scene/rendering/CommandBuffer.h"
#include <GL/glew.h>
#include <iostream>

namespace core {

    OpenGLBackend::OpenGLBackend(Window& win) : window(&win) {
        // Initialize matrices
        projectionMatrix = math::Mat4::identity();
        viewMatrix = math::Mat4::identity();
    }

    OpenGLBackend::~OpenGLBackend() {
        shutdown();
    }

    bool OpenGLBackend::init(uint32_t w, uint32_t h) {
        this->width = w;
        this->height = h;

        // Set initial viewport
        setViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

        // Set up default projection matrix (2D orthographic)
        projectionMatrix = math::Mat4::ortho(0.0f, static_cast<float>(width),
            static_cast<float>(height), 0.0f,
            -1.0f, 1.0f);

        // Set up identity view matrix
        viewMatrix = math::Mat4::identity();

        // Set default clear color
        setDrawColor(0.0f, 0.0f, 0.0f, 1.0f);

        initialized = true;
        return true;
    }

    bool OpenGLBackend::beginFrame() {
        if (!initialized) {
            return false;
        }

        clear();
        return true;
    }

    bool OpenGLBackend::submit(const scene::CommandBuffer& commandBuffer) {
        if (!initialized) {
            return false;
        }

        // For now, we'll implement basic logging similar to NullBackend
        // but with actual OpenGL calls for basic operations

        // Get camera and apply matrices
        const auto& camera = commandBuffer.getCamera();
        // Convert math::Matrix4f to math::Mat4 (they're the same type, just different aliases)
        setProjectionMatrix(camera.projectionMatrix);
        setViewMatrix(camera.viewMatrix);

        // Process draw items (basic logging for now)
        const auto& drawItems = commandBuffer.getDrawItems();
        if (!drawItems.empty()) {
            std::cout << "[OpenGLBackend] Processing " << drawItems.size() << " draw items" << std::endl;
            // TODO: Implement actual mesh/material rendering
        }

        // Process UI items (basic logging for now)
        const auto& uiItems = commandBuffer.getUIItems();
        if (!uiItems.empty()) {
            std::cout << "[OpenGLBackend] Processing " << uiItems.size() << " UI items" << std::endl;
            // TODO: Implement actual UI rendering
        }

        // Process post effects (basic logging for now)
        const auto& postEffects = commandBuffer.getPostEffects();
        if (!postEffects.empty()) {
            std::cout << "[OpenGLBackend] Processing " << postEffects.size() << " post effects" << std::endl;
            // TODO: Implement actual post-processing
        }

        return true;
    }

    bool OpenGLBackend::present() {
        if (!initialized || !window) {
            return false;
        }

        window->swapBuffers();
        return true;
    }

    void OpenGLBackend::shutdown() {
        if (initialized) {
            initialized = false;
        }
    }

    bool OpenGLBackend::resize(uint32_t newWidth, uint32_t newHeight) {
        if (!initialized) {
            return false;
        }

        width = newWidth;
        height = newHeight;

        setViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

        // Update projection matrix
        projectionMatrix = math::Mat4::ortho(0.0f, static_cast<float>(width),
            static_cast<float>(height), 0.0f,
            -1.0f, 1.0f);

        return true;
    }

    std::string OpenGLBackend::getBackendType() const {
        return "OpenGL";
    }

    bool OpenGLBackend::isInitialized() const {
        return initialized;
    }

    // Additional methods for compatibility
    void OpenGLBackend::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLBackend::setDrawColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void OpenGLBackend::setViewport(int x, int y, int w, int h) {
        glViewport(x, y, w, h);
    }

    void OpenGLBackend::getViewportSize(int& w, int& h) const {
        w = static_cast<int>(width);
        h = static_cast<int>(height);
    }

    void OpenGLBackend::setProjectionMatrix(const math::Mat4& projection) {
        projectionMatrix = projection;
    }

    void OpenGLBackend::setViewMatrix(const math::Mat4& view) {
        viewMatrix = view;
    }

}

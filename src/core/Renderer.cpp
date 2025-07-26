#include "Renderer.h"
#include "Window.h"

namespace core {

    Renderer::Renderer(Window& window) {
        // Create and initialize the OpenGL backend
        backend = std::make_unique<OpenGLBackend>(window);

        // Get window size for initialization
        int width, height;
        window.getSize(width, height);

        // Initialize the backend
        backend->init(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void Renderer::clear() {
        backend->clear();
    }

    void Renderer::present() {
        backend->present();
    }

    void Renderer::setDrawColor(float r, float g, float b, float a) {
        backend->setDrawColor(r, g, b, a);
    }

    void Renderer::setViewport(int x, int y, int width, int height) {
        backend->setViewport(x, y, width, height);
    }

    void Renderer::getViewportSize(int& width, int& height) const {
        backend->getViewportSize(width, height);
    }

    void Renderer::setProjectionMatrix(const math::Mat4& projection) {
        backend->setProjectionMatrix(projection);
    }

    void Renderer::setViewMatrix(const math::Mat4& view) {
        backend->setViewMatrix(view);
    }

    const math::Mat4& Renderer::getProjectionMatrix() const {
        return backend->getProjectionMatrix();
    }

    const math::Mat4& Renderer::getViewMatrix() const {
        return backend->getViewMatrix();
    }

}
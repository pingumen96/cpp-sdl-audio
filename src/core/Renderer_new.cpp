#include "Renderer.h"
#include "Window.h"
#include <iostream>

namespace core {

    Renderer::Renderer(Window& window) : window(&window) {
        try {
            adapter = std::make_unique<compat::RendererAdapter>(window);
            std::cout << "[Renderer] Initialized with new stratified architecture" << std::endl;
            std::cout << "[Renderer] " << adapter->get2DRenderer()->getRendererInfo() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Renderer] Failed to initialize: " << e.what() << std::endl;
            throw;
        }
    }

    Renderer::~Renderer() {
        // adapter will be automatically destroyed
    }

    void Renderer::clear() {
        if (adapter) adapter->clear();
    }

    void Renderer::present() {
        if (adapter) adapter->present();
    }

    void Renderer::setDrawColor(float r, float g, float b, float a) {
        if (adapter) adapter->setDrawColor(r, g, b, a);
    }

    void Renderer::setViewport(int x, int y, int width, int height) {
        if (adapter) adapter->setViewport(x, y, width, height);
    }

    void Renderer::getViewportSize(int& width, int& height) const {
        if (adapter) {
            adapter->getViewportSize(width, height);
        } else {
            width = height = 0;
        }
    }

    void Renderer::setProjectionMatrix(const math::Mat4& projection) {
        if (adapter) adapter->setProjectionMatrix(projection);
    }

    void Renderer::setViewMatrix(const math::Mat4& view) {
        if (adapter) adapter->setViewMatrix(view);
    }

    const math::Mat4& Renderer::getProjectionMatrix() const {
        if (adapter) {
            return adapter->getProjectionMatrix();
        }
        static const math::Mat4 identity = math::Mat4::identity();
        return identity;
    }

    const math::Mat4& Renderer::getViewMatrix() const {
        if (adapter) {
            return adapter->getViewMatrix();
        }
        static const math::Mat4 identity = math::Mat4::identity();
        return identity;
    }

    rendering::twod::Renderer2D* Renderer::get2DRenderer() const {
        if (adapter) return adapter->get2DRenderer();
        return nullptr;
    }

    rendering::backend::RenderBackend* Renderer::getBackend() const {
        if (adapter) return adapter->getBackend();
        return nullptr;
    }

    scene::IRenderBackend* Renderer::getRenderBackend() const {
        // For legacy compatibility, we need to create a bridge
        // For now, return nullptr and log a deprecation warning
        std::cerr << "[Renderer] WARNING: getRenderBackend() is deprecated. Use getBackend() instead." << std::endl;
        return nullptr;
    }

    void Renderer::drawRect(const rendering::twod::Rect2D& rect, const rendering::twod::Color& color) {
        if (adapter) adapter->drawRect(rect, color);
    }

    void Renderer::drawTexturedRect(const rendering::twod::Rect2D& rect, rendering::backend::TextureHandle texture) {
        if (adapter) adapter->drawTexturedRect(rect, texture);
    }

} // namespace core

#include "RendererAdapter.h"
#include "../core/Window.h"

namespace compat {

    RendererAdapter::RendererAdapter(core::Window& window)
        : window(&window) {

        // Get window dimensions
        int width, height;
        window.getSize(width, height);

        // Create the new 2D renderer
        renderer2D = rendering::createRenderer2D(window,
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height));

        if (!renderer2D) {
            throw std::runtime_error("Failed to create 2D renderer");
        }

        // Initialize default matrices
        currentProjection = math::Mat4::identity();
        currentView = math::Mat4::identity();
    }

    RendererAdapter::~RendererAdapter() {
        // renderer2D will be automatically destroyed
    }

    void RendererAdapter::clear() {
        if (!renderer2D) return;

        renderer2D->beginFrame();
        renderer2D->clear(rendering::twod::Color(currentR, currentG, currentB, currentA));
    }

    void RendererAdapter::present() {
        if (!renderer2D) return;

        renderer2D->endFrame();
    }

    void RendererAdapter::setDrawColor(float r, float g, float b, float a) {
        currentR = r;
        currentG = g;
        currentB = b;
        currentA = a;
    }

    void RendererAdapter::setViewport(int x, int y, int width, int height) {
        if (!renderer2D) return;

        renderer2D->setViewportSize(static_cast<float>(width), static_cast<float>(height));
    }

    void RendererAdapter::getViewportSize(int& width, int& height) const {
        if (!renderer2D) {
            width = height = 0;
            return;
        }

        float w, h;
        renderer2D->getViewportSize(w, h);
        width = static_cast<int>(w);
        height = static_cast<int>(h);
    }

    void RendererAdapter::setProjectionMatrix(const math::Mat4& projection) {
        currentProjection = projection;
        projectionSet = true;

        // The new renderer handles matrices differently - we'll need to adapt this
        // For now, we store it and apply it when needed
    }

    void RendererAdapter::setViewMatrix(const math::Mat4& view) {
        currentView = view;
        viewSet = true;

        // The new renderer uses camera position instead of view matrix
        // We could extract translation from the view matrix here
    }

    const math::Mat4& RendererAdapter::getProjectionMatrix() const {
        return currentProjection;
    }

    const math::Mat4& RendererAdapter::getViewMatrix() const {
        return currentView;
    }

    rendering::backend::RenderBackend* RendererAdapter::getBackend() const {
        if (!renderer2D) return nullptr;
        return renderer2D->getBackend();
    }

    void RendererAdapter::drawRect(const rendering::twod::Rect2D& rect, const rendering::twod::Color& color) {
        if (!renderer2D) return;
        renderer2D->drawRect(rect, color);
    }

    void RendererAdapter::drawTexturedRect(const rendering::twod::Rect2D& rect, rendering::backend::TextureHandle texture) {
        if (!renderer2D) return;
        renderer2D->drawTexturedRect(rect, texture);
    }

} // namespace compat

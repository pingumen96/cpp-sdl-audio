#include "Renderer.h"
#include "Window.h"

core::Renderer::Renderer(Window& win) : window(&win) {
    // Get initial viewport size
    window->getSize(viewportWidth, viewportHeight);

    // Set initial viewport
    setViewport(0, 0, viewportWidth, viewportHeight);

    // Set up default projection matrix (2D orthographic)
    projectionMatrix = math::Mat4::ortho(0.0f, static_cast<float>(viewportWidth),
        static_cast<float>(viewportHeight), 0.0f,
        -1.0f, 1.0f);

    // Set up identity view matrix
    viewMatrix = math::Mat4::identity();

    // Set default clear color
    setDrawColor(0.0f, 0.0f, 0.0f, 1.0f);
}void core::Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void core::Renderer::present() {
    window->swapBuffers();
}

void core::Renderer::setDrawColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void core::Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    viewportWidth = width;
    viewportHeight = height;
}

void core::Renderer::getViewportSize(int& width, int& height) const {
    width = viewportWidth;
    height = viewportHeight;
}

void core::Renderer::setProjectionMatrix(const math::Mat4& projection) {
    projectionMatrix = projection;
}

void core::Renderer::setViewMatrix(const math::Mat4& view) {
    viewMatrix = view;
}
#include "Rendering.h"
#include "../core/Window.h"

namespace rendering {

    std::unique_ptr<twod::Renderer2D> createRenderer2D(
        core::Window& window,
        uint32_t width,
        uint32_t height,
        bool useNullDevice
    ) {
        // Create HAL device
        std::unique_ptr<hal::RenderDevice> halDevice;

        if (useNullDevice) {
            halDevice = std::make_unique<hal::NullDevice>();
        } else {
            halDevice = std::make_unique<hal::OpenGLDevice>(window);
        }

        // Create backend
        auto backend = std::make_unique<backend::RenderBackend>(std::move(halDevice));

        // Create 2D renderer
        auto renderer = std::make_unique<twod::Renderer2D>(std::move(backend));

        // Initialize the complete stack
        if (!renderer->initialize(width, height)) {
            return nullptr;
        }

        return renderer;
    }

    std::unique_ptr<twod::Renderer2D> createNullRenderer2D(
        uint32_t width,
        uint32_t height
    ) {
        // Create null HAL device
        auto halDevice = std::make_unique<hal::NullDevice>();

        // Create backend
        auto backend = std::make_unique<backend::RenderBackend>(std::move(halDevice));

        // Create 2D renderer
        auto renderer = std::make_unique<twod::Renderer2D>(std::move(backend));

        // Initialize the complete stack
        if (!renderer->initialize(width, height)) {
            return nullptr;
        }

        return renderer;
    }

} // namespace rendering

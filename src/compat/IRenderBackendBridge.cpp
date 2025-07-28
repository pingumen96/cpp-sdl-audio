#include "IRenderBackendBridge.h"

namespace compat {

    IRenderBackendBridge::IRenderBackendBridge(rendering::backend::RenderBackend* backend)
        : backend(backend) {}

    bool IRenderBackendBridge::init(uint32_t width, uint32_t height) {
        // The new backend is already initialized through the HAL device
        // We just store the dimensions
        return true;
    }

    bool IRenderBackendBridge::beginFrame() {
        if (backend) {
            backend->beginFrame();
            return true;
        }
        return false;
    }

    bool IRenderBackendBridge::submit(const scene::CommandBuffer& commandBuffer) {
        if (!backend) return false;

        // For now, we'll return true as a placeholder
        // A full implementation would convert scene::CommandBuffer to rendering::backend::CommandBuffer
        return true;
    }

    bool IRenderBackendBridge::present() {
        if (backend) {
            backend->endFrame();
            return true;
        }
        return false;
    }

    void IRenderBackendBridge::shutdown() {
        // The backend is owned by the HAL device, we don't shut it down here
    }

    bool IRenderBackendBridge::resize(uint32_t width, uint32_t height) {
        // Resize would be handled by the HAL device
        return true;
    }

    std::string IRenderBackendBridge::getBackendType() const {
        if (backend) {
            return "New Architecture Backend";
        }
        return "Null Backend";
    }

    bool IRenderBackendBridge::isInitialized() const {
        return backend != nullptr;
    }

} // namespace compat

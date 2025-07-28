#pragma once

#include "../scene/rendering/IRenderBackend.h"
#include "../rendering/backend/RenderBackend.h"
#include <memory>

namespace compat {

    /**
     * @brief Bridge between new rendering::backend::RenderBackend and legacy scene::IRenderBackend
     *
     * This adapter allows the new rendering architecture to work with existing scene system
     * code that expects the legacy IRenderBackend interface.
     */
    class IRenderBackendBridge : public scene::IRenderBackend {
    private:
        rendering::backend::RenderBackend* backend;

    public:
        explicit IRenderBackendBridge(rendering::backend::RenderBackend* backend);
        virtual ~IRenderBackendBridge() = default;

        // scene::IRenderBackend interface implementation
        bool init(uint32_t width, uint32_t height) override;
        bool beginFrame() override;
        bool submit(const scene::CommandBuffer& commandBuffer) override;
        bool present() override;
        void shutdown() override;
        bool resize(uint32_t width, uint32_t height) override;
        std::string getBackendType() const override;
        bool isInitialized() const override;
    };

} // namespace compat

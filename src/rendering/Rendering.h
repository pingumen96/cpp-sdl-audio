#pragma once

/**
 * @file Rendering.h
 * @brief Main header for the new stratified rendering architecture
 *
 * This header provides access to the complete rendering system with clearly
 * separated layers:
 *
 * 1. HAL (Hardware Abstraction Layer) - rendering/hal/
 * 2. Backend (Command & Resource Management) - rendering/backend/
 * 3. 2D Frontend (High-level 2D API) - rendering/2d/
 * 4. Scene Integration (ECS integration) - scene/rendering/
 */

 // ===== LEVEL 1: Hardware Abstraction Layer =====
#include "hal/RenderDevice.h"
#include "hal/opengl/OpenGLDevice.h"
#include "hal/null/NullDevice.h"

// ===== LEVEL 2: Render Backend =====
#include "backend/CommandBuffer.h"
#include "backend/RenderBackend.h"

// ===== LEVEL 3: 2D Rendering Frontend =====
#include "2d/Renderer2D.h"

// Note: Level 4 (Scene Integration) is in scene/rendering/

namespace rendering {

    /**
     * @brief Factory to create a complete 2D rendering system
     * @param window Window to render to (for OpenGL context)
     * @param width Initial viewport width
     * @param height Initial viewport height
     * @param useNullDevice If true, uses null device for testing
     * @return Complete 2D renderer or nullptr if failed
     */
    std::unique_ptr<twod::Renderer2D> createRenderer2D(
        core::Window& window,
        uint32_t width,
        uint32_t height,
        bool useNullDevice = false
    );

    /**
     * @brief Create a null renderer for testing
     * @param width Initial viewport width
     * @param height Initial viewport height
     * @return Null renderer for headless testing
     */
    std::unique_ptr<twod::Renderer2D> createNullRenderer2D(
        uint32_t width,
        uint32_t height
    );

} // namespace rendering

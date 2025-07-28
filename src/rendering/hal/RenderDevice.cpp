#include "RenderDevice.h"
#include "opengl/OpenGLDevice.h"
#include "null/NullDevice.h"

namespace rendering::hal {

    std::unique_ptr<RenderDevice> createDefaultRenderDevice() {
        // For now, we'll create OpenGL device if we have a valid context
        // In the future, this could check for available APIs and choose the best one

#ifdef RENDER_NULL_DEVICE
        return std::make_unique<NullDevice>();
#else
    // This assumes we have a valid window context
    // In practice, you'd pass the window here or get it from somewhere
        return nullptr; // Will be created with window in actual usage
#endif
    }

} // namespace rendering::hal

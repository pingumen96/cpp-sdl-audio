#pragma once

namespace ecs {

    /**
     * @brief Global flags resource for application state
     *
     * Contains various application-wide flags that systems can read and modify.
     */
    struct GlobalFlags {
        bool quit = false;          // Application should quit
        bool paused = false;        // Application is paused
        bool debugMode = false;     // Debug mode enabled

        // Add other global flags as needed
    };

} // namespace ecs

#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace scene {

    /**
     * @brief Unique identifier for ECS worlds
     */
    using WorldID = std::uint32_t;

    /**
     * @brief Invalid/uninitialized world ID
     */
    static constexpr WorldID INVALID_WORLD_ID = 0;

    /**
     * @brief Forward declarations
     */
    class Scene;
    class SceneNode;
    class SceneManager;
    class IRenderBackend;
    class RenderQueueBuilder;
    class SceneTransition;

    /**
     * @brief Smart pointer types
     */
    using ScenePtr = std::unique_ptr<Scene>;
    using SceneNodePtr = std::unique_ptr<SceneNode>;
    using TransitionPtr = std::unique_ptr<SceneTransition>;

} // namespace scene

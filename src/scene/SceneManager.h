#pragma once

#include "SceneTypes.h"
#include "Scene.h"
#include "SceneTransition.h"
#include "rendering/IRenderBackend.h"
#include "rendering/RenderQueueBuilder.h"
#include "../resources/ResourceSystem.h"
#include <vector>
#include <memory>
#include <atomic>

namespace scene {

    /**
     * @brief Main scene management system
     *
     * SceneManager manages a stack of scenes, handles transitions,
     * coordinates updates and rendering, and manages resource loading
     * through the ResourceManager.
     */
    class SceneManager {
    private:
        // Scene stack
        std::vector<ScenePtr> sceneStack;

        // Rendering
        RenderBackendPtr renderBackend;
        RenderQueueBuilder renderBuilder;

        // Resources
        std::unique_ptr<resources::ResourceManager> resourceManager;

        // Transitions
        TransitionPtr currentTransition;

        // World ID generation
        std::atomic<WorldID> nextWorldId{ 1 };

        // State
        bool initialized = false;
        uint32_t renderWidth = 1920;
        uint32_t renderHeight = 1080;

    public:
        /**
         * @brief Constructor
         * @param backend Render backend to use
         */
        explicit SceneManager(RenderBackendPtr backend = nullptr);

        /**
         * @brief Destructor
         */
        ~SceneManager();

        // Non-copyable but movable
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = default;
        SceneManager& operator=(SceneManager&&) = default;

        /**
         * @brief Initialize the scene manager
         * @param width Initial render width
         * @param height Initial render height
         * @return true if initialization succeeded
         */
        bool initialize(uint32_t width = 1920, uint32_t height = 1080);

        /**
         * @brief Shutdown the scene manager
         */
        void shutdown();

        // Scene management methods

        /**
         * @brief Push a new scene onto the stack
         * @param newScene Scene to push
         * @param transition Optional transition to use
         */
        void pushScene(ScenePtr newScene, TransitionPtr transition = nullptr);

        /**
         * @brief Pop the top scene from the stack
         * @param transition Optional transition to use
         * @return The popped scene (for reuse if needed)
         */
        ScenePtr popScene(TransitionPtr transition = nullptr);

        /**
         * @brief Switch to a new scene (replaces current top scene)
         * @param newScene Scene to switch to
         * @param transition Optional transition to use
         * @return The replaced scene (for reuse if needed)
         */
        ScenePtr switchScene(ScenePtr newScene, TransitionPtr transition = nullptr);

        /**
         * @brief Clear all scenes from the stack
         */
        void clearScenes();

        // Update and render methods

        /**
         * @brief Update all scenes and transitions
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime);

        /**
         * @brief Render all scenes
         * @return true if rendering succeeded
         */
        bool render();

        /**
         * @brief Handle input (forwards to scenes from top to bottom)
         * @param inputData Input data to process
         * @return true if input was consumed
         */
        template<typename InputData>
        bool handleInput(const InputData& inputData);

        // Getters and state queries

        /**
         * @brief Get the current top scene
         * @return Pointer to top scene or nullptr if stack is empty
         */
        Scene* getCurrentScene() const;

        /**
         * @brief Get scene at specific index
         * @param index Index in the stack (0 = bottom, size-1 = top)
         * @return Pointer to scene or nullptr if index is invalid
         */
        Scene* getScene(size_t index) const;

        /**
         * @brief Get the number of scenes in the stack
         */
        size_t getSceneCount() const { return sceneStack.size(); }

        /**
         * @brief Check if scene manager is initialized
         */
        bool isInitialized() const { return initialized; }

        /**
         * @brief Check if there's an active transition
         */
        bool hasActiveTransition() const { return currentTransition != nullptr; }

        /**
         * @brief Get current transition (if any)
         */
        SceneTransition* getCurrentTransition() const { return currentTransition.get(); }

        /**
         * @brief Get resource manager
         */
        resources::ResourceManager* getResourceManager() const { return resourceManager.get(); }

        /**
         * @brief Get render backend
         */
        IRenderBackend* getRenderBackend() const { return renderBackend.get(); }

        /**
         * @brief Get current render size
         */
        std::pair<uint32_t, uint32_t> getRenderSize() const { return { renderWidth, renderHeight }; }

        /**
         * @brief Resize render target
         * @param width New width
         * @param height New height
         * @return true if resize succeeded
         */
        bool resize(uint32_t width, uint32_t height);

        // Debug and utility methods

        /**
         * @brief Get debug information about the scene stack
         */
        std::string getDebugInfo() const;

        /**
         * @brief Get render statistics
         */
        struct RenderStats {
            size_t totalDrawItems = 0;
            size_t totalPostEffects = 0;
            size_t totalUIItems = 0;
            size_t commandBufferSize = 0;
        };

        RenderStats getLastRenderStats() const;

    private:
        /**
         * @brief Generate a new unique world ID
         */
        WorldID generateWorldId() { return nextWorldId++; }

        /**
         * @brief Setup a scene after adding it to the stack
         */
        void setupScene(Scene* scene);

        /**
         * @brief Cleanup a scene before removing it from the stack
         */
        void cleanupScene(Scene* scene);

        /**
         * @brief Update current transition
         * @param deltaTime Time elapsed since last update
         */
        void updateTransition(float deltaTime);

        /**
         * @brief Get camera parameters for rendering
         */
        CameraParams getCurrentCamera() const;

        /**
         * @brief Get render target for current frame
         */
        RenderTarget getCurrentRenderTarget() const;

        // Last render stats for debugging
        mutable RenderStats lastRenderStats;
    };

    // Template implementation for handleInput
    template<typename InputData>
    bool SceneManager::handleInput(const InputData& inputData) {
        // Process scenes from top to bottom until one consumes the input
        for (auto it = sceneStack.rbegin(); it != sceneStack.rend(); ++it) {
            Scene* scene = it->get();
            if (scene && !scene->isPaused()) {
                // Scene would need to implement input handling
                // This is a placeholder for the interface
                // if (scene->handleInput(inputData)) {
                //     return true;
                // }

                // If scene pauses underlying scenes, stop here
                if (scene->getPausesUnderlying()) {
                    break;
                }
            }
        }
        return false;
    }

} // namespace scene

#pragma once

#include "SceneTypes.h"
#include "rendering/RenderQueueBuilder.h"
#include "../resources/ResourceSystem.h"

namespace scene {

    // Forward declaration
    class Scene;

    /**
     * @brief Abstract base class for scene transitions
     *
     * SceneTransitions are strategy objects that handle the visual
     * and loading aspects of switching between scenes.
     */
    class SceneTransition {
    protected:
        Scene* oldScene = nullptr;
        Scene* newScene = nullptr;
        float progress = 0.0f;
        bool completed = false;

    public:
        virtual ~SceneTransition() = default;

        /**
         * @brief Begin the transition
         * @param fromScene Scene being transitioned from (can be nullptr)
         * @param toScene Scene being transitioned to
         */
        virtual void begin(Scene* fromScene, Scene* toScene) {
            oldScene = fromScene;
            newScene = toScene;
            progress = 0.0f;
            completed = false;
        }

        /**
         * @brief Update the transition
         * @param deltaTime Time elapsed since last update
         * @return true if transition is complete
         */
        virtual bool update(float deltaTime) = 0;

        /**
         * @brief Render transition overlay
         * @param builder RenderQueueBuilder for adding transition visuals
         */
        virtual void renderOverlay(RenderQueueBuilder& builder) = 0;

        /**
         * @brief Get transition progress (0.0 to 1.0)
         */
        float getProgress() const { return progress; }

        /**
         * @brief Check if transition is complete
         */
        bool isCompleted() const { return completed; }

        /**
         * @brief Get transition type name for debugging
         */
        virtual std::string getTransitionType() const = 0;

    protected:
        /**
         * @brief Mark transition as completed
         */
        void setCompleted() {
            progress = 1.0f;
            completed = true;
        }

        /**
         * @brief Update progress and check for completion
         * @param newProgress New progress value (0.0 to 1.0)
         */
        void updateProgress(float newProgress) {
            progress = std::clamp(newProgress, 0.0f, 1.0f);
            if (progress >= 1.0f) {
                completed = true;
            }
        }
    };

} // namespace scene

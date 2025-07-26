#pragma once

#include "SceneTransition.h"
#include <algorithm>

namespace scene {

    /**
     * @brief Instant transition (no visual effect)
     */
    class InstantTransition : public SceneTransition {
    public:
        bool update(float deltaTime) override {
            setCompleted();
            return true;
        }

        void renderOverlay(RenderQueueBuilder& builder) override {
            // No visual overlay for instant transition
        }

        std::string getTransitionType() const override {
            return "InstantTransition";
        }
    };

    /**
     * @brief Simple fade transition
     */
    class FadeTransition : public SceneTransition {
    private:
        float duration = 1.0f;
        float fadeAlpha = 0.0f;
        bool fadingOut = true;

    public:
        /**
         * @brief Constructor with duration
         */
        explicit FadeTransition(float transitionDuration = 1.0f)
            : duration(transitionDuration) {}

        void begin(Scene* fromScene, Scene* toScene) override {
            SceneTransition::begin(fromScene, toScene);
            fadeAlpha = 0.0f;
            fadingOut = (fromScene != nullptr);
        }

        bool update(float deltaTime) override {
            if (completed) return true;

            float progressDelta = deltaTime / duration;

            if (fadingOut) {
                // Fade out old scene
                fadeAlpha += progressDelta;
                if (fadeAlpha >= 1.0f) {
                    fadeAlpha = 1.0f;
                    fadingOut = false;
                }
                updateProgress(fadeAlpha * 0.5f); // First half of transition
            } else {
                // Fade in new scene
                fadeAlpha -= progressDelta;
                if (fadeAlpha <= 0.0f) {
                    fadeAlpha = 0.0f;
                    setCompleted();
                }
                updateProgress(0.5f + (1.0f - fadeAlpha) * 0.5f); // Second half
            }

            return completed;
        }

        void renderOverlay(RenderQueueBuilder& builder) override {
            if (fadeAlpha > 0.0f) {
                // Add a full-screen black quad with alpha
                UIItem fadeQuad;
                fadeQuad.position = glm::vec2(0.0f, 0.0f);
                fadeQuad.size = glm::vec2(1.0f, 1.0f); // Full screen
                fadeQuad.textureId = ""; // Solid color
                fadeQuad.color = glm::vec4(0.0f, 0.0f, 0.0f, fadeAlpha);
                fadeQuad.layer = 1000; // High layer to render on top

                builder.enqueue(fadeQuad);
            }
        }

        std::string getTransitionType() const override {
            return "FadeTransition";
        }
    };

    /**
     * @brief Progress bar transition for loading
     */
    class LoadingTransition : public SceneTransition {
    private:
        float minDisplayTime = 0.5f; // Minimum time to show loading screen
        float elapsed = 0.0f;
        resources::ResourceManager* resourceManager = nullptr;

    public:
        /**
         * @brief Constructor with minimum display time
         */
        explicit LoadingTransition(float minTime = 0.5f, resources::ResourceManager* resManager = nullptr)
            : minDisplayTime(minTime), resourceManager(resManager) {}

        void begin(Scene* fromScene, Scene* toScene) override {
            SceneTransition::begin(fromScene, toScene);
            elapsed = 0.0f;
        }

        bool update(float deltaTime) override {
            if (completed) return true;

            elapsed += deltaTime;

            // Update loading progress if resource manager is available
            if (resourceManager) {
                // Call updateLoadingJobs() to advance async operations
                // This would be implemented in ResourceManager to process
                // background loading tasks
                // resourceManager->updateLoadingJobs();
            }

            // Check if minimum time has passed and loading is complete
            bool minTimeElapsed = (elapsed >= minDisplayTime);
            bool loadingComplete = true; // Would check actual loading state

            // For now, simulate loading completion based on time
            float loadingProgress = std::min(elapsed / 2.0f, 1.0f);
            updateProgress(loadingProgress);

            if (minTimeElapsed && loadingComplete && progress >= 1.0f) {
                setCompleted();
            }

            return completed;
        }

        void renderOverlay(RenderQueueBuilder& builder) override {
            // Background
            UIItem background;
            background.position = glm::vec2(0.0f, 0.0f);
            background.size = glm::vec2(1.0f, 1.0f);
            background.textureId = "";
            background.color = glm::vec4(0.1f, 0.1f, 0.1f, 0.9f);
            background.layer = 999;
            builder.enqueue(background);

            // Progress bar background
            UIItem progressBg;
            progressBg.position = glm::vec2(0.25f, 0.45f);
            progressBg.size = glm::vec2(0.5f, 0.05f);
            progressBg.textureId = "";
            progressBg.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            progressBg.layer = 1000;
            builder.enqueue(progressBg);

            // Progress bar fill
            UIItem progressFill;
            progressFill.position = glm::vec2(0.25f, 0.45f);
            progressFill.size = glm::vec2(0.5f * progress, 0.05f);
            progressFill.textureId = "";
            progressFill.color = glm::vec4(0.2f, 0.8f, 0.2f, 1.0f);
            progressFill.layer = 1001;
            builder.enqueue(progressFill);

            // Loading text (would need text rendering system)
            // For now, just add a placeholder
            UIItem loadingText;
            loadingText.position = glm::vec2(0.4f, 0.3f);
            loadingText.size = glm::vec2(0.2f, 0.1f);
            loadingText.textureId = "loading_text";
            loadingText.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            loadingText.layer = 1002;
            builder.enqueue(loadingText);
        }

        std::string getTransitionType() const override {
            return "LoadingTransition";
        }

        /**
         * @brief Set resource manager for monitoring loading progress
         */
        void setResourceManager(resources::ResourceManager* resManager) {
            resourceManager = resManager;
        }
    };

    /**
     * @brief Cross-fade transition
     */
    class CrossFadeTransition : public SceneTransition {
    private:
        float duration = 1.0f;

    public:
        explicit CrossFadeTransition(float transitionDuration = 1.0f)
            : duration(transitionDuration) {}

        bool update(float deltaTime) override {
            if (completed) return true;

            float progressDelta = deltaTime / duration;
            updateProgress(progress + progressDelta);

            return completed;
        }

        void renderOverlay(RenderQueueBuilder& builder) override {
            // Cross-fade would require rendering both scenes
            // and blending them based on progress
            // This is a simplified version that just shows alpha overlay

            if (progress > 0.0f && progress < 1.0f) {
                UIItem overlay;
                overlay.position = glm::vec2(0.0f, 0.0f);
                overlay.size = glm::vec2(1.0f, 1.0f);
                overlay.textureId = "";
                overlay.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f * (1.0f - progress));
                overlay.layer = 998;
                builder.enqueue(overlay);
            }
        }

        std::string getTransitionType() const override {
            return "CrossFadeTransition";
        }
    };

} // namespace scene

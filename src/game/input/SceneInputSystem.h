#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include "Command.h"
#include "../../scene/Scene.h"

namespace game::input {

    /**
     * @brief Unified input system that integrates with scene management
     *
     * This system replaces the old InputHandler and provides scene-aware
     * input handling with command pattern support.
     */
    class SceneInputSystem {
    private:
        std::unordered_map<int, std::unique_ptr<Command>> globalBindings;

        // Scene-specific input contexts
        struct SceneInputContext {
            std::unordered_map<int, std::unique_ptr<Command>> bindings;
            bool capturesAllInput = false; // If true, don't propagate to global
        };

        std::unordered_map<std::string, SceneInputContext> sceneContexts;

    public:
        /**
         * @brief Bind a key to a global command (works in all scenes)
         */
        void bindGlobal(int key, std::unique_ptr<Command> command);

        /**
         * @brief Bind a key to a scene-specific command
         */
        void bindForScene(const std::string& sceneName, int key, std::unique_ptr<Command> command);

        /**
         * @brief Set whether a scene captures all input (no global fallback)
         */
        void setSceneCapturesInput(const std::string& sceneName, bool captures);

        /**
         * @brief Handle input event for the current active scene
         */
        bool handleInput(const SDL_Event& event, scene::Scene* currentScene);

        /**
         * @brief Load key bindings from JSON file
         */
        void loadBindings(const std::string& filename);

        /**
         * @brief Clear all bindings for a specific scene
         */
        void clearSceneBindings(const std::string& sceneName);

    private:
        /**
         * @brief Execute command if bound to the given key
         */
        bool executeCommand(int key, const std::unordered_map<int, std::unique_ptr<Command>>& bindings);
    };

} // namespace game::input

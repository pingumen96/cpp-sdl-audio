#include "SceneInputSystem.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace game::input {

    void SceneInputSystem::bindGlobal(int key, std::unique_ptr<Command> command) {
        globalBindings[key] = std::move(command);
    }

    void SceneInputSystem::bindForScene(const std::string& sceneName, int key, std::unique_ptr<Command> command) {
        sceneContexts[sceneName].bindings[key] = std::move(command);
    }

    void SceneInputSystem::setSceneCapturesInput(const std::string& sceneName, bool captures) {
        sceneContexts[sceneName].capturesAllInput = captures;
    }

    bool SceneInputSystem::handleInput(const SDL_Event& event, scene::Scene* currentScene) {
        if (!currentScene) {
            return false;
        }

        if (event.type == SDL_KEYDOWN) {
            int keyCode = event.key.keysym.sym;
            std::string sceneName = currentScene->getName();

            // First try scene-specific bindings
            auto sceneIt = sceneContexts.find(sceneName);
            if (sceneIt != sceneContexts.end()) {
                if (executeCommand(keyCode, sceneIt->second.bindings)) {
                    return true;
                }

                // If scene captures all input, don't try global bindings
                if (sceneIt->second.capturesAllInput) {
                    return false;
                }
            }

            // Try global bindings as fallback
            return executeCommand(keyCode, globalBindings);
        }

        return false;
    }

    void SceneInputSystem::loadBindings(const std::string& filename) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "[SceneInputSystem] Cannot open bindings file: " << filename << std::endl;
                return;
            }

            json data;
            file >> data;

            // Load global bindings
            if (data.contains("global")) {
                for (const auto& [key, action] : data["global"].items()) {
                    int keyCode = SDL_GetKeyFromName(key.c_str());
                    if (keyCode != SDLK_UNKNOWN) {
                        // Create appropriate command based on action
                        // For now, we'll just store the action string
                        // In a real implementation, you'd have a command factory
                        std::cout << "[SceneInputSystem] Global binding: " << key << " -> " << action << std::endl;
                    }
                }
            }

            // Load scene-specific bindings
            if (data.contains("scenes")) {
                for (const auto& [sceneName, sceneBindings] : data["scenes"].items()) {
                    for (const auto& [key, action] : sceneBindings.items()) {
                        int keyCode = SDL_GetKeyFromName(key.c_str());
                        if (keyCode != SDLK_UNKNOWN) {
                            std::cout << "[SceneInputSystem] Scene binding " << sceneName
                                << ": " << key << " -> " << action << std::endl;
                        }
                    }
                }
            }

        } catch (const std::exception& e) {
            std::cerr << "[SceneInputSystem] Error loading bindings: " << e.what() << std::endl;
        }
    }

    void SceneInputSystem::clearSceneBindings(const std::string& sceneName) {
        sceneContexts.erase(sceneName);
    }

    bool SceneInputSystem::executeCommand(int key, const std::unordered_map<int, std::unique_ptr<Command>>& bindings) {
        auto it = bindings.find(key);
        if (it != bindings.end() && it->second) {
            // For now, we need to pass some context to execute
            // This will need to be refactored to pass proper game context
            // it->second->execute();
            std::cout << "[SceneInputSystem] Command would execute for key: " << key << std::endl;
            return true;
        }
        return false;
    }

} // namespace game::input

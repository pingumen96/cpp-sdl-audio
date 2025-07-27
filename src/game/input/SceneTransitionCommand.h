#pragma once

#include "Command.h"
#include "../../scene/SceneManager.h"
#include <memory>
#include <functional>

namespace game::input {

    /**
     * @brief Command for scene transitions
     *
     * This command encapsulates scene transitions, allowing them to be bound
     * to input keys through the unified input system.
     */
    class SceneTransitionCommand : public Command {
    private:
        std::function<std::unique_ptr<scene::Scene>()> sceneFactory;
        scene::SceneManager* sceneManager;
        bool pushMode; // true = push, false = switch

    public:
        /**
         * @brief Create a scene transition command
         * @param manager Scene manager to operate on
         * @param factory Function that creates the target scene
         * @param push If true, push scene onto stack; if false, switch to scene
         */
        SceneTransitionCommand(scene::SceneManager* manager,
            std::function<std::unique_ptr<scene::Scene>()> factory,
            bool push = false)
            : sceneFactory(std::move(factory)), sceneManager(manager), pushMode(push) {}

        void execute() override {
            if (sceneManager && sceneFactory) {
                auto newScene = sceneFactory();
                if (newScene) {
                    if (pushMode) {
                        sceneManager->pushScene(std::move(newScene));
                    } else {
                        sceneManager->switchScene(std::move(newScene));
                    }
                }
            }
        }
    };

} // namespace game::input

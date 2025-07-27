#pragma once

#include "Command.h"
#include "../../scene/Scene.h"

namespace game::input {

    /**
     * @brief Base class for scene-specific commands
     */
    class SceneCommand : public Command {
    protected:
        scene::Scene* targetScene;

    public:
        explicit SceneCommand(scene::Scene* scene) : targetScene(scene) {}
        virtual ~SceneCommand() = default;
    };

    /**
     * @brief Command to handle menu navigation
     */
    class MenuNavigateCommand : public SceneCommand {
    private:
        int direction; // -1 for up, 1 for down

    public:
        MenuNavigateCommand(scene::Scene* scene, int dir)
            : SceneCommand(scene), direction(dir) {}

        void execute() override {
            // This will be handled by the menu scene directly
            // through handleMenuInput
        }
    };

    /**
     * @brief Command to handle player movement
     */
    class PlayerMoveCommand : public SceneCommand {
    private:
        int keyCode;

    public:
        PlayerMoveCommand(scene::Scene* scene, int key)
            : SceneCommand(scene), keyCode(key) {}

        void execute() override {
            // This will be handled by the game scene directly
            // through handleGameInput
        }
    };

} // namespace game::input

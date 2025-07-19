#pragma once

#include <unordered_map>
#include <memory>

#include "Command.h"
#include "../Avatar.h"

namespace game::input {
    class InputHandler {
        std::unordered_map<int, std::unique_ptr<Command>> keyBindings;

    public:
        void bind(int key, std::unique_ptr<Command> command) {
            keyBindings[key] = std::move(command);
        }

        void handleInput(int key, Avatar& avatar) {
            if (keyBindings.count(key)) {
                keyBindings[key]->execute(avatar);
            }
        }

        void loadBindings(const std::string& filename) {}
    };
}
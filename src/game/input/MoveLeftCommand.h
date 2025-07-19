#pragma once

#include "Command.h"
#include "../Avatar.h"

namespace game::input {
    class MoveLeftCommand : public Command {

    public:
        void execute(Avatar& avatar) override {
            avatar.moveLeft();
        }
    };
}
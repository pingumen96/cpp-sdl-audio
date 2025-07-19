#pragma once

#include "Command.h"
#include "MoveLeftCommand.h"
#include "../Avatar.h"
#include <iostream>
#include <memory>


namespace game::input {
    class MoveLeftCommand : public Command {

    public:
        void execute(Avatar& avatar) override {
            avatar.moveLeft();
        }
    };
}
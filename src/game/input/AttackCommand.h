#pragma once

#include "Command.h"
#include "../Avatar.h"

namespace game::input {
    class AttackCommand : public Command {
    public:
        void execute(Avatar& avatar) override {
            avatar.attack();
        }
    };
}

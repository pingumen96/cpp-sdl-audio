#pragma once

#include "Command.h"
#include "../Avatar.h"

namespace game { class Avatar; }

namespace game::input {
    class JumpCommand : public Command {
    public:
        void execute(Avatar& avatar) override;
    };
}

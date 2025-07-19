#pragma once

#include "Command.h"

#include <iostream>

namespace game { class Avatar; }

namespace game::input {

    class PauseCommand : public Command {
    public:
        void execute(Avatar& avatar) override;
    };
}

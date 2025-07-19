#pragma once

#include "Command.h"

namespace game { class Avatar; }


namespace game::input {
    class MoveLeftCommand : public Command {

    public:
        void execute(Avatar& avatar) override;
    };
}
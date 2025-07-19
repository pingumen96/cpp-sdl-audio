#pragma once

#include "../Avatar.h"


namespace game::input {
    class Command {
    public:
        virtual ~Command() = default;
        virtual void execute(Avatar& avatar) = 0;
    };

}


#pragma once

#include "../Avatar.h"


namespace game::input {
    class Command {
    public:
        virtual ~Command() = default;
        virtual void execute(Avatar& avatar) = 0;
        virtual void undo(Avatar& avatar) {
            // Default implementation does nothing
            // Derived classes can override this if needed
        }
    };

}


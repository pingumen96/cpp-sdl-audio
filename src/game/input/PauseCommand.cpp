#include "PauseCommand.h"

#include "../Avatar.h"

#include <iostream>

namespace game::input {

    void PauseCommand::execute(Avatar& avatar) {
        std::cout << "[PauseCommand] Executing pause command\n";
    }

} // namespace game::input

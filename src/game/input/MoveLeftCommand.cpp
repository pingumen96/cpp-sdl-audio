#include "MoveLeftCommand.h"

#include "../Avatar.h"

namespace game::input {

    void MoveLeftCommand::execute(Avatar& avatar) {
        std::cout << "[MoveLeftCommand] Executing move left command\n";
        avatar.moveLeft();
    }
} // namespace game::input
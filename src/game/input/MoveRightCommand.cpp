#include "MoveRightCommand.h"

#include "../Avatar.h"

namespace game::input {

    void MoveRightCommand::execute(Avatar& avatar) {
        std::cout << "[MoveRightCommand] Executing move right command\n";
        avatar.moveRight();
    }

} // namespace game::input

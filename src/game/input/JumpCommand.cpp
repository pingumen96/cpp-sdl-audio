#include "JumpCommand.h"
#include "../Avatar.h"

namespace game::input {

    void JumpCommand::execute(Avatar& avatar) {
        std::cout << "[JumpCommand] Executing jump command\n";
        avatar.jump();
    }

} // namespace game::input
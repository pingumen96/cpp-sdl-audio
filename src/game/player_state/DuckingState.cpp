#include "DuckingState.h"

#include "../Avatar.h"
#include "StandingState.h"

namespace game {
    void DuckingState::moveLeft(Avatar& avatar) {
        std::cout << "[DuckingState] Moving left\n";
        avatar.applyHorizontalVelocity(-1.0f);
    }

    void DuckingState::moveRight(Avatar& avatar) {
        std::cout << "[DuckingState] Moving right\n";
        avatar.applyHorizontalVelocity(1.0f);
    }

    void DuckingState::jump(Avatar& avatar) {
        std::cout << "[DuckingState] Cannot jump while ducking\n";
    }

    void DuckingState::standUp(Avatar& avatar) {
        std::cout << "[DuckingState] Standing up\n";
        avatar.setState<StandingState>();
    }

    void DuckingState::attack(Avatar& avatar) {
        std::cout << "[DuckingState] Attacking while ducking\n";
    }
}
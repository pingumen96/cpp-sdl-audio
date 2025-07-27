#include <iostream>

#include "StandingState.h"
#include "../Avatar.h"
#include "DuckingState.h"
#include "AirborneState.h"

namespace game {
    StandingState::~StandingState() = default;
    void StandingState::moveLeft(Avatar& avatar) {
        std::cout << "[StandingState] Moving left\n";
        avatar.applyHorizontalVelocity(-1.0f);
    }

    void StandingState::moveRight(Avatar& avatar) {
        std::cout << "[StandingState] Moving right\n";
        avatar.applyHorizontalVelocity(1.0f);
    }

    void StandingState::jump(Avatar& avatar) {
        std::cout << "[StandingState] Jumping\n";
        avatar.doJumpImpulse();
        avatar.setState<AirborneState>();
    }

    void StandingState::attack(Avatar& avatar) {
        std::cout << "[StandingState] Attacking\n";
    }

    void StandingState::duck(Avatar& avatar) {
        std::cout << "[StandingState] Ducking\n";
        avatar.setState<DuckingState>();
    }
}
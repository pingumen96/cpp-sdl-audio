#include "AirborneState.h"

#include "../Avatar.h"
#include "StandingState.h"
#include "DuckingState.h"


namespace game {
    void AirborneState::moveLeft(Avatar& avatar) {
        std::cout << "[AirborneState] Moving left\n";
        avatar.applyHorizontalVelocity(-1.0f);
    }

    void AirborneState::moveRight(Avatar& avatar) {
        std::cout << "[AirborneState] Moving right\n";
        avatar.applyHorizontalVelocity(1.0f);
    }

    void AirborneState::attack(Avatar& avatar) {
        std::cout << "[AirborneState] Attacking\n";
    }

    void AirborneState::onLanded(Avatar& avatar) {
        std::cout << "[AirborneState] Landed\n";
        avatar.setState<StandingState>();
    }
}
// StandingState.h
#pragma once
#include "PlayerState.h"
#include <iostream>

namespace game {
    class StandingState : public PlayerState {
    public:
        ~StandingState() override;
        const char* name() const override { return "Standing"; }

        void moveLeft(Avatar& avatar) override;
        void moveRight(Avatar& avatar) override;
        void jump(Avatar& avatar) override;
        void attack(Avatar& avatar) override;
        void duck(Avatar& avatar) override;
    };
}

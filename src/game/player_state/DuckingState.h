// DuckingState.h
#pragma once
#include "PlayerState.h"
#include <iostream>
namespace game {
    class DuckingState : public PlayerState {
    public:
        const char* name() const override { return "Ducking"; }

        void moveLeft(Avatar& avatar) override;
        void moveRight(Avatar& avatar) override;
        void jump(Avatar& avatar) override;
        void standUp(Avatar& avatar) override;
        void attack(Avatar& avatar) override;
    };
}
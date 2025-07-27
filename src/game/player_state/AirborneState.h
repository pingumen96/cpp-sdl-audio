// AirborneState.h
#pragma once
#include "PlayerState.h"
#include <iostream>
namespace game {
    class AirborneState : public PlayerState {
    public:
        const char* name() const override { return "Airborne"; }

        void moveLeft(Avatar& avatar) override;
        void moveRight(Avatar& avatar) override;
        void attack(Avatar& avatar) override;
        ~AirborneState() override;
        void onLanded(Avatar& avatar) override;
    };
}
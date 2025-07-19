#pragma once

// src/game/Avatar.h – Avatar class for player character

#include <iostream>
#include <string>

namespace game {

    class Avatar {
    public:
        void moveLeft() { std::cout << "Moving left\n"; }
        void moveRight() { std::cout << "Moving right\n"; }
        void jump() { std::cout << "Jumping\n"; }
        void attack() { std::cout << "Attacking\n"; }
    private:
    };
}

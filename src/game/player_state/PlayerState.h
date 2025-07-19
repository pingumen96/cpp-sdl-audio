// PlayerState.h
#pragma once
#include <string>

namespace game {
    class Avatar; // forward

    class PlayerState {
    public:
        virtual ~PlayerState() = default;

        virtual const char* name() const = 0;


        // cppcheck-suppress unusedParameter
        virtual void moveLeft(Avatar& avatar) { /* default: ignorare o muovere */ }

        // cppcheck-suppress unusedParameter
        virtual void moveRight(Avatar& avatar) { /* default */ }

        // cppcheck-suppress unusedParameter
        virtual void jump(Avatar& avatar) { /* default */ }

        // cppcheck-suppress unusedParameter
        virtual void attack(Avatar& avatar) { /* default */ }

        // cppcheck-suppress unusedParameter
        virtual void duck(Avatar& avatar) { /* default */ }

        // cppcheck-suppress unusedParameter
        virtual void standUp(Avatar& avatar) { /* default */ }

        // Hook per update fisica (se serve)
        virtual void update(Avatar& avatar, float dt) {}
        // Eventi “ambientali” (aterrato)
        virtual void onLanded(Avatar& avatar) {}
    };
}

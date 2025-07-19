// PlayerState.h
#pragma once
#include <string>

namespace game {
    class Avatar; // forward

    class PlayerState {
    public:
        virtual ~PlayerState() = default;

        virtual const char* name() const = 0;

        // Azioni “intenzionali” provenienti dai comandi
        virtual void moveLeft(Avatar& avatar) { /* default: ignorare o muovere */ }
        virtual void moveRight(Avatar& avatar) { /* default */ }
        virtual void jump(Avatar& avatar) { /* default */ }
        virtual void attack(Avatar& avatar) { /* default */ }
        virtual void duck(Avatar& avatar) { /* default */ }
        virtual void standUp(Avatar& avatar) { /* default */ }

        // Hook per update fisica (se serve)
        virtual void update(Avatar& avatar, float dt) {}
        // Eventi “ambientali” (aterrato)
        virtual void onLanded(Avatar& avatar) {}
    };
}

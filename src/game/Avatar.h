// Avatar.h
#pragma once
#include <iostream>
#include <memory>
#include "player_state/PlayerState.h"
#include "player_state/StandingState.h"

namespace game {

    class Avatar {
    public:
        Avatar() {
            setState<StandingState>();
        }

        template<typename State, typename...Args>
        void setState(Args&&...args) {
            state = std::make_unique<State>(std::forward<Args>(args)...);
            std::cout << "[Avatar] State -> " << state->name() << "\n";
        }

        const char* currentStateName() const {
            return state ? state->name() : "(none)";
        }

        // API chiamata dai Command
        void moveLeft() { state->moveLeft(*this); }
        void moveRight() { state->moveRight(*this); }
        void jump() { state->jump(*this); }
        void attack() { state->attack(*this); }
        void duck() { state->duck(*this); }
        void standUp() { state->standUp(*this); }

        // Eventi ambientali (es: rilevi atterraggio nella fisica)
        void landed() { state->onLanded(*this); }

        // Logica futura di movimento, fisica ecc.:
        void applyHorizontalVelocity(float dir) {
            std::cout << "Applying velocity dir=" << dir << "\n";
        }
        void doJumpImpulse() {
            std::cout << "Applying jump impulse\n";
        }

    private:
        std::unique_ptr<PlayerState> state;

        // Consentiamo agli stati di cambiare stato (friend opzionale)
        friend class StandingState;
        friend class DuckingState;
        friend class AirborneState;
    };
}

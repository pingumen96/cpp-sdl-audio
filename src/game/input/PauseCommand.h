#pragma once

#include "Command.h"
#include "../Avatar.h"

#include <iostream>
#include <memory>

namespace game::input {

    class PauseCommand : public Command {
    public:
        void execute(Avatar& avatar) override {
            // Logica per mettere in pausa il gioco
            std::cout << "Pausing game\n";
        }
    };
}

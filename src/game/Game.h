#pragma once

#include <memory>
#include "input/InputHandler.h"

namespace game {


    class Game {
    public:
        void init() {
            // ... roba varia ...
            inputHandler = std::make_unique<input::InputHandler>();

            inputHandler->loadBindings("data/input_bindings.json");
        }

        void mainLoop() {
            // nothing for now
        }

    private:
        std::unique_ptr<input::InputHandler> inputHandler;
    };
}
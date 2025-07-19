#pragma once

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <iostream>


#include "Command.h"
#include "MoveLeftCommand.h"
#include "MoveRightCommand.h"
#include "JumpCommand.h"
#include "PauseCommand.h"


namespace game { class Avatar; }

using json = nlohmann::json;

namespace game::input {

    // Helper per mappare stringhe a SDL_Keycode
    inline int stringToKeycode(const std::string& key) {
        int keycode = SDL_GetKeyFromName(key.c_str());
        if (keycode != SDLK_UNKNOWN) return keycode;
        throw std::runtime_error("Key binding non riconosciuto: " + key);
    }

    class InputHandler {
        std::unordered_map<int, std::unique_ptr<Command>> keyBindings;

    public:
        void bind(int key, std::unique_ptr<Command> command);

        void handleInput(int key, Avatar& avatar);

        void loadBindings(const std::string& filename, Avatar& avatar);
    };
}

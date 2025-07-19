#pragma once

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <iostream>

#include "Command.h"
#include "../Avatar.h"
#include "MoveLeftCommand.h"
#include "MoveRightCommand.h"
#include "JumpCommand.h"
#include "PauseCommand.h"

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
        void bind(int key, std::unique_ptr<Command> command) {
            keyBindings[key] = std::move(command);
        }

        void handleInput(int key, Avatar& avatar) {
            std::cout << "Handling input for key: " << SDL_GetKeyName(key) << " (" << key << ")\n";
            if (keyBindings.count(key)) {
                keyBindings[key]->execute(avatar);
            }
        }

        void loadBindings(const std::string& filename, Avatar& avatar) {
            std::ifstream f(filename);
            if (!f.is_open()) {
                throw std::runtime_error("Impossibile aprire il file: " + filename);
            }
            json j;
            f >> j;

            for (auto& [action, keyStr] : j.items()) {
                int keycode = stringToKeycode(keyStr);

                if (action == "move_left") {
                    bind(keycode, std::make_unique<MoveLeftCommand>());
                } else if (action == "move_right") {
                    bind(keycode, std::make_unique<MoveRightCommand>());
                } else if (action == "jump") {
                    bind(keycode, std::make_unique<JumpCommand>());
                } else if (action == "pause") {
                    bind(keycode, std::make_unique<PauseCommand>());
                } else {
                    throw std::runtime_error("Azione non riconosciuta: " + action);
                }
            }
        }
    };
}

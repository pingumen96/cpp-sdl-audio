
#include "InputHandler.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include "../Avatar.h"
#include "Command.h"
#include "MoveLeftCommand.h"
#include "MoveRightCommand.h"
#include "JumpCommand.h"
#include "PauseCommand.h"

using json = nlohmann::json;

void game::input::InputHandler::handleInput(int key, game::Avatar& avatar) {
    std::cout << "Handling input for key: " << SDL_GetKeyName(key) << " (" << key << ")\n";
    if (keyBindings.count(key)) {
        keyBindings[key]->execute(avatar);
    }
}

void game::input::InputHandler::loadBindings(const std::string& filename, game::Avatar& avatar) {
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

void game::input::InputHandler::bind(int key, std::unique_ptr<Command> command) {
    keyBindings[key] = std::move(command);
}
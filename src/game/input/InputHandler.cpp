#include "InputHandler.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <functional>
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

// Factory map: azione -> funzione che restituisce unique_ptr<Command>
namespace {
    using CommandFactory = std::function<std::unique_ptr<game::input::Command>()>;

    const std::unordered_map<std::string, CommandFactory> commandFactories = {
        { "move_left",  [] { return std::make_unique<game::input::MoveLeftCommand>(); } },
        { "move_right", [] { return std::make_unique<game::input::MoveRightCommand>(); } },
        { "jump",       [] { return std::make_unique<game::input::JumpCommand>(); } },
        { "pause",      [] { return std::make_unique<game::input::PauseCommand>(); } }
    };
}

void game::input::InputHandler::loadBindings(const std::string& filename, game::Avatar& avatar) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        throw std::runtime_error("Impossibile aprire il file: " + filename);
    }
    json j;
    f >> j;

    std::for_each(j.begin(), j.end(), [this](const auto& pair) {
        const std::string& action = pair.first;
        const std::string& keyStr = pair.second;
        int keycode = stringToKeycode(keyStr);

        auto it = commandFactories.find(action);
        if (it != commandFactories.end()) {
            bind(keycode, it->second());
        } else {
            throw std::runtime_error("Azione non riconosciuta: " + action);
        }
        });
}

void game::input::InputHandler::bind(int key, std::unique_ptr<Command> command) {
    keyBindings[key] = std::move(command);
}

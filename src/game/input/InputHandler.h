#pragma once

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <SDL2/SDL.h>

#include "Command.h"
#include "../Avatar.h"
#include "MoveLeftCommand.h"
#include "MoveRightCommand.h"
#include "JumpCommand.h"
#include "AttackCommand.h"

using json = nlohmann::json;

namespace game::input {

    // Helper per mappare stringhe a SDL_Keycode
    inline int stringToKeycode(const std::string& key) {
        if (key == "A") return SDLK_a;
        if (key == "D") return SDLK_d;
        if (key == "SPACE") return SDLK_SPACE;
        if (key == "K") return SDLK_k;
        // Aggiungi qui altri mapping, o usa SDL_GetKeyFromName
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
            std::cout << "Handling input for key: " << key << "\n";
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

            // Assumi struttura tipo: { "move_left": "A", ... }
            for (auto& [action, keyStr] : j.items()) {
                int keycode = stringToKeycode(keyStr);

                // Collega l'azione al comando corrispondente
                if (action == "move_left") {
                    bind(keycode, std::make_unique<MoveLeftCommand>());
                } else if (action == "move_right") {
                    bind(keycode, std::make_unique<MoveRightCommand>());
                } else if (action == "jump") {
                    bind(keycode, std::make_unique<JumpCommand>());
                } else if (action == "attack") {
                    bind(keycode, std::make_unique<AttackCommand>());
                } else {
                    // Qui puoi gestire azioni aggiuntive o dare errore
                    throw std::runtime_error("Azione non riconosciuta: " + action);
                }
            }
        }
    };
}

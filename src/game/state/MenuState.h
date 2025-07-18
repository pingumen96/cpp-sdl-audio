// MenuState.h
#pragma once
#include "GameState.h"
#include <vector>
#include <string>

namespace game {
    class Game; // forward declaration
    class MenuState : public GameState {
        int selected = 0;
        std::vector<std::string> entries = { "Start", "Options", "Quit" };
    public:
        const char* name() const override { return "Menu"; }
        void handleEvent(Game& game, const SDL_Event& ev) override;
        void render(Game& game) override;
    };
}
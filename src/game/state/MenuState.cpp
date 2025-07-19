#include "MenuState.h"
#include "../Game.h"
#include "PlayingState.h"

void game::MenuState::handleEvent(game::Game& game, const SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
        case SDLK_UP:
            selected = (selected - 1 + (int)entries.size()) % entries.size();
            break;
        case SDLK_DOWN:
            selected = (selected + 1) % entries.size();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (entries[selected] == "Start") {
                game.setState<game::PlayingState>();
            } else if (entries[selected] == "Quit") {
                SDL_Event quit;
                quit.type = SDL_QUIT;
                SDL_PushEvent(&quit);
            }
            break;
        }
    }
}

void game::MenuState::render(Game& game) {
    // placeholder rendering logic
    std::cout << "[Menu] Rendering Menu State\n";
    std::cout << "[Menu] Selected: " << entries[selected] << "\n";
}



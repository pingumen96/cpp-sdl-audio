#pragma once

#include <memory>
#include "input/InputHandler.h"
#include "Avatar.h"

#include "state/GameState.h"
#include "state/MenuState.h"
#include "state/PlayingState.h"
#include "state/PausedState.h"

namespace game {
    class GameState;
    class MenuState;
    class Game {
    public:
        void init() {
            setState<MenuState>();
            // ... roba varia ...
            inputHandler = std::make_unique<input::InputHandler>();

            inputHandler->loadBindings("data/keybindings.json", avatar);
        }

        template<typename State, typename...Args>
        void setState(Args&&...args) {
            state = std::make_unique<State>(std::forward<Args>(args)...);
            std::cout << "[Game] State -> " << state->name() << "\n";
        }

        void mainLoop() {
            constexpr Uint32 FPS = 60;
            constexpr Uint32 FRAME_DELAY = 1000 / FPS;
            Uint32 lastTicks = SDL_GetTicks();
            bool running = true;

            while (running) {
                Uint32 frameStart = SDL_GetTicks();
                float dt = (frameStart - lastTicks) / 1000.0f;
                lastTicks = frameStart;

                // --- input handling ---
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                    } else if (state) {
                        state->handleEvent(*this, event);
                    }
                }

                if (state) {
                    state->update(*this, dt);
                }

                if (state) {
                    state->render(*this);
                } else {
                    std::cerr << "[Game] No active state to render!\n";
                }

                // --- frame timing ---
                Uint32 frameTime = SDL_GetTicks() - frameStart;
                if (frameTime < FRAME_DELAY) {
                    SDL_Delay(FRAME_DELAY - frameTime);
                }
            }
        }

        input::InputHandler& getInputHandler() {
            return *inputHandler;
        }
        Avatar& getAvatar() {
            return avatar;
        }
    private:
        std::unique_ptr<GameState> state; // Stato di gioco attivo
        std::unique_ptr<input::InputHandler> inputHandler;
        Avatar avatar; // Il tuo avatar di gioco
    };
}
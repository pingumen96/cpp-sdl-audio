#pragma once

#include <memory>
#include "input/InputHandler.h"
#include "Avatar.h"

namespace game {
    class Game {
    public:
        void init() {
            // ... roba varia ...
            inputHandler = std::make_unique<input::InputHandler>();

            inputHandler->loadBindings("data/input_bindings.json", avatar);
        }

        void mainLoop() {
            constexpr Uint32 FPS = 60;
            constexpr Uint32 FRAME_DELAY = 1000 / FPS;
            bool running = true;

            while (running) {
                Uint32 frameStart = SDL_GetTicks();

                // --- input handling ---
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                    } else if (event.type == SDL_KEYDOWN) {
                        // Passa l'evento al gestore input
                        inputHandler->handleInput(event.key.keysym.sym, avatar);
                    }
                }

                // --- game logic ---
                // Qui puoi aggiungere la logica di gioco, come aggiornamenti dell'avatar

                // --- rendering ---
                // Qui puoi aggiungere il codice per il rendering della scena

                // --- frame timing ---
                Uint32 frameTime = SDL_GetTicks() - frameStart;
                if (frameTime < FRAME_DELAY) {
                    SDL_Delay(FRAME_DELAY - frameTime);
                }
            }
        }

    private:
        std::unique_ptr<input::InputHandler> inputHandler;
        Avatar avatar; // Il tuo avatar di gioco
    };
}
#pragma once

#include "../System.h"
#include "../InputState.h"
#include "../GlobalFlags.h"
#include "../EventBus.h"
#include "../events/InputEvents.h"
#include <SDL2/SDL.h>

namespace ecs::systems {

    /**
     * @brief System that collects raw input from SDL and updates InputState
     *
     * This system should run first in the system pipeline every frame.
     * It polls SDL events and updates the global InputState resource.
     */
    class InputCollectSystem : public System {
    private:
        InputState* mInputState = nullptr;
        GlobalFlags* mGlobalFlags = nullptr;
        EventBus* mEventBus = nullptr;

    public:
        /**
         * @brief Initialize the system with resource pointers from coordinator
         */
        void init(InputState* inputState, GlobalFlags* globalFlags, EventBus* eventBus) {
            mInputState = inputState;
            mGlobalFlags = globalFlags;
            mEventBus = eventBus;
        }

        /**
         * @brief Update method called every frame
         * @param deltaTime Time elapsed since last frame
         */
        void update(float deltaTime) override {
            if (!mInputState || !mGlobalFlags || !mEventBus) {
                return;
            }

            // Clear frame-specific input data
            mInputState->clearFrameData();

            // Poll SDL events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                handleSDLEvent(event);
            }
        }

    private:
        /**
         * @brief Handle individual SDL events
         */
        void handleSDLEvent(const SDL_Event& event) {
            // Emit raw event for systems that need direct SDL access
            mEventBus->emit<events::RawInputEvent>(event);

            switch (event.type) {
            case SDL_QUIT:
                mGlobalFlags->quit = true;
                mEventBus->emit<events::SceneTransition>(events::SceneTransition::Type::QUIT);
                break;

            case SDL_KEYDOWN:
                if (event.key.repeat == 0) { // Ignore key repeats
                    mInputState->handleKeyDown(event.key.keysym.scancode);
                }
                break;

            case SDL_KEYUP:
                mInputState->handleKeyUp(event.key.keysym.scancode);
                break;

            case SDL_MOUSEBUTTONDOWN:
                mInputState->handleMouseButtonDown(event.button.button);
                break;

            case SDL_MOUSEBUTTONUP:
                mInputState->handleMouseButtonUp(event.button.button);
                break;

            case SDL_MOUSEMOTION:
                mInputState->handleMouseMotion(
                    event.motion.x,
                    event.motion.y,
                    event.motion.xrel,
                    event.motion.yrel
                );
                break;

            case SDL_MOUSEWHEEL:
                mInputState->handleMouseWheel(event.wheel.y);
                break;

            default:
                // Other events are ignored for now
                break;
            }
        }
    };

} // namespace ecs::systems

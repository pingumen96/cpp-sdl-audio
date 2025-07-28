#pragma once

#include "../System.h"
#include "../InputState.h"
#include "../EventBus.h"
#include "../events/InputEvents.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <cmath>

namespace ecs::systems {

    /**
     * @brief System that maps raw input to high-level intents
     *
     * Reads InputState and converts device input to game intents,
     * emitting them on the EventBus. Uses a configurable mapping system.
     */
    class InputMappingSystem : public System {
    private:
        InputState* mInputState = nullptr;
        EventBus* mEventBus = nullptr;

        // Input mapping configuration
        struct InputBinding {
            std::function<void(InputMappingSystem*)> action;
        };

        std::unordered_map<SDL_Scancode, InputBinding> mKeyBindings;

    public:
        /**
         * @brief Initialize the system with resource pointers from coordinator
         */
        void init(InputState* inputState, EventBus* eventBus) {
            mInputState = inputState;
            mEventBus = eventBus;

            // Set up default key bindings (hard-coded for now)
            setupDefaultBindings();
        }

        /**
         * @brief Update method called every frame
         * @param deltaTime Time elapsed since last frame
         */
        void update(float deltaTime) override {
            if (!mInputState || !mEventBus) {
                return;
            }

            // Process movement input
            processMovementInput();

            // Process key presses
            for (SDL_Scancode key : mInputState->pressedThisFrame) {
                auto it = mKeyBindings.find(key);
                if (it != mKeyBindings.end()) {
                    it->second.action(this);
                }
            }
        }

    private:
        /**
         * @brief Set up default key bindings
         */
        void setupDefaultBindings() {
            // Movement keys are handled separately in processMovementInput()

            // Fire/Attack
            mKeyBindings[SDL_SCANCODE_SPACE] = { [](InputMappingSystem* self) {
                self->mEventBus->emit<events::FireIntent>(0.0f, -1.0f, true);
            } };

            // UI Navigation
            mKeyBindings[SDL_SCANCODE_RETURN] = { [](InputMappingSystem* self) {
                self->mEventBus->emit<events::UiSelect>(events::UiSelect::Type::CONFIRM);
            } };

            mKeyBindings[SDL_SCANCODE_KP_ENTER] = { [](InputMappingSystem* self) {
                self->mEventBus->emit<events::UiSelect>(events::UiSelect::Type::CONFIRM);
            } };

            mKeyBindings[SDL_SCANCODE_ESCAPE] = { [](InputMappingSystem* self) {
                self->mEventBus->emit<events::SceneTransition>(events::SceneTransition::Type::BACK);
            } };

            mKeyBindings[SDL_SCANCODE_TAB] = { [](InputMappingSystem* self) {
                self->mEventBus->emit<events::SceneTransition>(events::SceneTransition::Type::TO_GAME);
            } };

            mKeyBindings[SDL_SCANCODE_P] = { [](InputMappingSystem* self) {
                self->mEventBus->emit<events::PauseToggle>();
            } };
        }

        /**
         * @brief Process continuous movement input
         */
        void processMovementInput() {
            float moveX = 0.0f;
            float moveY = 0.0f;

            // WASD movement
            if (mInputState->isKeyPressed(SDL_SCANCODE_A) || mInputState->isKeyPressed(SDL_SCANCODE_LEFT)) {
                moveX -= 1.0f;
            }
            if (mInputState->isKeyPressed(SDL_SCANCODE_D) || mInputState->isKeyPressed(SDL_SCANCODE_RIGHT)) {
                moveX += 1.0f;
            }
            if (mInputState->isKeyPressed(SDL_SCANCODE_W) || mInputState->isKeyPressed(SDL_SCANCODE_UP)) {
                moveY -= 1.0f;
            }
            if (mInputState->isKeyPressed(SDL_SCANCODE_S) || mInputState->isKeyPressed(SDL_SCANCODE_DOWN)) {
                moveY += 1.0f;
            }

            // Emit movement intent if there's any movement
            if (moveX != 0.0f || moveY != 0.0f) {
                // Normalize diagonal movement
                if (moveX != 0.0f && moveY != 0.0f) {
                    float length = std::sqrt(moveX * moveX + moveY * moveY);
                    moveX /= length;
                    moveY /= length;
                }

                mEventBus->emit<events::MoveIntent>(moveX, moveY);
            }
        }

    public:
        /**
         * @brief Add or modify a key binding
         * @param key The scancode to bind
         * @param action The action to perform when the key is pressed
         */
        void setKeyBinding(SDL_Scancode key, std::function<void(InputMappingSystem*)> action) {
            mKeyBindings[key] = { action };
        }

        /**
         * @brief Remove a key binding
         * @param key The scancode to unbind
         */
        void removeKeyBinding(SDL_Scancode key) {
            mKeyBindings.erase(key);
        }

        /**
         * @brief Clear all key bindings
         */
        void clearBindings() {
            mKeyBindings.clear();
        }
    };

} // namespace ecs::systems

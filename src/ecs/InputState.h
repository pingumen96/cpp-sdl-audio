#pragma once

#include <bitset>
#include <vector>
#include <SDL2/SDL.h>

namespace ecs {

    /**
     * @brief Global input state resource that tracks current input state
     *
     * This resource maintains the complete input state including:
     * - Currently pressed keys (bitmap)
     * - Keys pressed/released this frame (vectors)
     * - Mouse button states
     * - Mouse position and delta
     */
    struct InputState {
        // Keyboard state
        std::bitset<SDL_NUM_SCANCODES> currentKeys;     // Currently pressed keys
        std::vector<SDL_Scancode> pressedThisFrame;     // Keys pressed this frame
        std::vector<SDL_Scancode> releasedThisFrame;    // Keys released this frame

        // Mouse state
        struct MouseState {
            bool leftButton = false;
            bool rightButton = false;
            bool middleButton = false;

            int x = 0;
            int y = 0;
            int deltaX = 0;
            int deltaY = 0;

            int wheel = 0;  // Mouse wheel delta
        } mouse;

        // Utility methods

        /**
         * @brief Check if a key is currently pressed
         */
        bool isKeyPressed(SDL_Scancode key) const {
            return currentKeys[key];
        }

        /**
         * @brief Check if a key was pressed this frame
         */
        bool wasKeyPressed(SDL_Scancode key) const {
            return std::find(pressedThisFrame.begin(), pressedThisFrame.end(), key) != pressedThisFrame.end();
        }

        /**
         * @brief Check if a key was released this frame
         */
        bool wasKeyReleased(SDL_Scancode key) const {
            return std::find(releasedThisFrame.begin(), releasedThisFrame.end(), key) != releasedThisFrame.end();
        }

        /**
         * @brief Clear frame-specific input data (pressed/released vectors)
         * Should be called at the beginning of each frame
         */
        void clearFrameData() {
            pressedThisFrame.clear();
            releasedThisFrame.clear();
            mouse.deltaX = 0;
            mouse.deltaY = 0;
            mouse.wheel = 0;
        }

        /**
         * @brief Handle key down event
         */
        void handleKeyDown(SDL_Scancode key) {
            if (!currentKeys[key]) {
                currentKeys[key] = true;
                pressedThisFrame.push_back(key);
            }
        }

        /**
         * @brief Handle key up event
         */
        void handleKeyUp(SDL_Scancode key) {
            if (currentKeys[key]) {
                currentKeys[key] = false;
                releasedThisFrame.push_back(key);
            }
        }

        /**
         * @brief Handle mouse button down event
         */
        void handleMouseButtonDown(Uint8 button) {
            switch (button) {
            case SDL_BUTTON_LEFT:   mouse.leftButton = true; break;
            case SDL_BUTTON_RIGHT:  mouse.rightButton = true; break;
            case SDL_BUTTON_MIDDLE: mouse.middleButton = true; break;
            }
        }

        /**
         * @brief Handle mouse button up event
         */
        void handleMouseButtonUp(Uint8 button) {
            switch (button) {
            case SDL_BUTTON_LEFT:   mouse.leftButton = false; break;
            case SDL_BUTTON_RIGHT:  mouse.rightButton = false; break;
            case SDL_BUTTON_MIDDLE: mouse.middleButton = false; break;
            }
        }

        /**
         * @brief Handle mouse motion event
         */
        void handleMouseMotion(int newX, int newY, int relX, int relY) {
            mouse.deltaX += relX;
            mouse.deltaY += relY;
            mouse.x = newX;
            mouse.y = newY;
        }

        /**
         * @brief Handle mouse wheel event
         */
        void handleMouseWheel(int wheelDelta) {
            mouse.wheel += wheelDelta;
        }
    };

} // namespace ecs

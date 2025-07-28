#pragma once

#include <SDL2/SDL.h>

namespace ecs::events {

    /**
     * @brief Base event structure
     */
    struct BaseEvent {
        float timestamp = 0.0f;  // When the event occurred (can be set by systems)
    };

    /**
     * @brief Movement intent event
     */
    struct MoveIntent : BaseEvent {
        float x = 0.0f;  // Normalized movement (-1.0 to 1.0)
        float y = 0.0f;

        MoveIntent() = default;
        MoveIntent(float x, float y) : x(x), y(y) {}
    };

    /**
     * @brief Fire/Attack intent event
     */
    struct FireIntent : BaseEvent {
        float x = 0.0f;  // Direction (normalized)
        float y = 0.0f;
        bool primary = true;  // Primary or secondary fire

        FireIntent() = default;
        FireIntent(float x, float y, bool primary = true) : x(x), y(y), primary(primary) {}
    };

    /**
     * @brief UI selection/confirmation event
     */
    struct UiSelect : BaseEvent {
        enum class Type {
            CONFIRM,
            CANCEL,
            MENU_UP,
            MENU_DOWN,
            MENU_LEFT,
            MENU_RIGHT
        };

        Type type = Type::CONFIRM;

        UiSelect() = default;
        UiSelect(Type type) : type(type) {}
    };

    /**
     * @brief Scene transition events
     */
    struct SceneTransition : BaseEvent {
        enum class Type {
            TO_MENU,
            TO_GAME,
            TO_PONG,
            BACK,
            QUIT
        };

        Type type = Type::TO_MENU;

        SceneTransition() = default;
        SceneTransition(Type type) : type(type) {}
    };

    /**
     * @brief Pause/Resume event
     */
    struct PauseToggle : BaseEvent {
        bool paused = false;

        PauseToggle() = default;
        PauseToggle(bool paused) : paused(paused) {}
    };

    /**
     * @brief Raw input event for systems that need direct SDL access
     */
    struct RawInputEvent : BaseEvent {
        SDL_Event sdlEvent;

        RawInputEvent() = default;
        RawInputEvent(const SDL_Event& event) : sdlEvent(event) {}
    };

} // namespace ecs::events

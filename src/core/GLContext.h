#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdexcept>

namespace core {
    class GLContext {
    private:
        SDL_GLContext context;
        bool initialized;

    public:
        GLContext(SDL_Window* window);
        ~GLContext();

        void makeCurrent(SDL_Window* window);
        void swapBuffers(SDL_Window* window);

        // Prevent copy
        GLContext(const GLContext&) = delete;
        GLContext& operator=(const GLContext&) = delete;

        // Allow move
        GLContext(GLContext&& other) noexcept;
        GLContext& operator=(GLContext&& other) noexcept;

        bool isInitialized() const { return initialized; }
    };
}

#pragma once

// src/core/Window.h – Window wrapper for SDL2 with OpenGL support
#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>
#include <string>
#include "GLContext.h"

namespace core {

    class Window {
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
        std::unique_ptr<GLContext> glContext;
    public:
        // Costruttori
        Window(const char* title, int x, int y, int w, int h, Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        Window(const std::string& title, int x, int y, int w, int h, Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

        // Accesso raw pointer
        SDL_Window* get() const;

        // Titolo
        void setTitle(const char* title) { SDL_SetWindowTitle(window.get(), title); }
        void setTitle(const std::string& title) { setTitle(title.c_str()); }

        // Dimensioni
        void setSize(int w, int h) { SDL_SetWindowSize(window.get(), w, h); }
        void getSize(int& w, int& h) const { SDL_GetWindowSize(window.get(), &w, &h); }

        // Posizione
        void setPosition(int x, int y) { SDL_SetWindowPosition(window.get(), x, y); }
        void getPosition(int& x, int& y) const { SDL_GetWindowPosition(window.get(), &x, &y); }

        // Stato finestra
        void show() { SDL_ShowWindow(window.get()); }
        void hide() { SDL_HideWindow(window.get()); }
        void minimize() { SDL_MinimizeWindow(window.get()); }
        void maximize() { SDL_MaximizeWindow(window.get()); }
        void restore() { SDL_RestoreWindow(window.get()); }

        // Fullscreen toggle
        void setFullscreen(bool enable) {
            SDL_SetWindowFullscreen(window.get(), enable ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        }
        bool isFullscreen() const {
            return (SDL_GetWindowFlags(window.get()) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
        }

        // Flags e ID
        Uint32 getFlags() const { return SDL_GetWindowFlags(window.get()); }
        Uint32 getID() const { return SDL_GetWindowID(window.get()); }

        // OpenGL context management
        GLContext& getGLContext() { return *glContext; }
        void swapBuffers() { glContext->swapBuffers(window.get()); }

        // Surface (utile se usi ancora SDL_Surface per rendering software)
        SDL_Surface* getSurface() const { return SDL_GetWindowSurface(window.get()); }
        void updateSurface() { SDL_UpdateWindowSurface(window.get()); }

        // Impostazione icona
        void setIcon(SDL_Surface* icon) { SDL_SetWindowIcon(window.get(), icon); }

        // Prevent copy
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        // Allow move
        Window(Window&&) = default;
        Window& operator=(Window&&) = default;
    };
}


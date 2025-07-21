#include "Window.h"

core::Window::Window(const char* title, int x, int y, int w, int h, Uint32 flags) : window(SDL_CreateWindow(title, x, y, w, h, flags), SDL_DestroyWindow) {
    if (!window) {
        throw std::runtime_error(SDL_GetError());
    }
}

core::Window::Window(const std::string& title, int x, int y, int w, int h, Uint32 flags)
    : Window(title.c_str(), x, y, w, h, flags) {}

SDL_Window* core::Window::get()  const { return window.get(); }
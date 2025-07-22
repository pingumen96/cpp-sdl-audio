#include "GLContext.h"
#include <iostream>

core::GLContext::GLContext(SDL_Window* window) : context(nullptr), initialized(false) {
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create context
    context = SDL_GL_CreateContext(window);
    if (!context) {
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    }

    // Make context current
    makeCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        SDL_GL_DeleteContext(context);
        throw std::runtime_error("Failed to initialize GLEW");
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initialized = true;

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

core::GLContext::~GLContext() {
    if (context) {
        SDL_GL_DeleteContext(context);
    }
}

void core::GLContext::makeCurrent(SDL_Window* window) {
    if (SDL_GL_MakeCurrent(window, context) < 0) {
        throw std::runtime_error("Failed to make OpenGL context current: " + std::string(SDL_GetError()));
    }
}

void core::GLContext::swapBuffers(SDL_Window* window) {
    SDL_GL_SwapWindow(window);
}

core::GLContext::GLContext(GLContext&& other) noexcept
    : context(other.context), initialized(other.initialized) {
    other.context = nullptr;
    other.initialized = false;
}

core::GLContext& core::GLContext::operator=(GLContext&& other) noexcept {
    if (this != &other) {
        if (context) {
            SDL_GL_DeleteContext(context);
        }
        context = other.context;
        initialized = other.initialized;
        other.context = nullptr;
        other.initialized = false;
    }
    return *this;
}

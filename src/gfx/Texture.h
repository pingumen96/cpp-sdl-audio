#pragma once

// src/gfx/Texture.h – OpenGL Texture wrapper
#include <GL/glew.h>
#include <SDL2/SDL_image.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace gfx {
    class Texture {
    private:
        GLuint textureID;
        int width, height;
        GLenum format;

        void loadFromSurface(SDL_Surface* surface);

    public:
        Texture(const char* file);
        Texture(const std::string& file) : Texture(file.c_str()) {}
        ~Texture();

        GLuint getID() const { return textureID; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }

        void bind(unsigned int unit = 0) const;
        void unbind() const;

        // Prevent copy
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Allow move
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
    };
}
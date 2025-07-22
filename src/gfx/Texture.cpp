#include "Texture.h"
#include <iostream>

gfx::Texture::Texture(const char* file) : textureID(0), width(0), height(0), format(GL_RGBA) {
    // Load image using SDL_image
    SDL_Surface* surface = IMG_Load(file);
    if (!surface) {
        throw std::runtime_error("Failed to load texture: " + std::string(IMG_GetError()));
    }

    loadFromSurface(surface);
    SDL_FreeSurface(surface);
}

gfx::Texture::~Texture() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

void gfx::Texture::loadFromSurface(SDL_Surface* surface) {
    width = surface->w;
    height = surface->h;

    // Determine format
    if (surface->format->BytesPerPixel == 4) {
        format = GL_RGBA;
    } else if (surface->format->BytesPerPixel == 3) {
        format = GL_RGB;
    } else {
        throw std::runtime_error("Unsupported image format");
    }

    // Generate and bind texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void gfx::Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void gfx::Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Move constructor
gfx::Texture::Texture(Texture&& other) noexcept
    : textureID(other.textureID), width(other.width), height(other.height), format(other.format) {
    other.textureID = 0;
    other.width = 0;
    other.height = 0;
}

// Move assignment operator
gfx::Texture& gfx::Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }
        textureID = other.textureID;
        width = other.width;
        height = other.height;
        format = other.format;

        other.textureID = 0;
        other.width = 0;
        other.height = 0;
    }
    return *this;
}
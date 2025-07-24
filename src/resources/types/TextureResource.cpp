#include "TextureResource.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdexcept>

namespace resources {

    TextureResource::TextureResource()
        : textureID(0), width(0), height(0), format(GL_RGB), loaded(false) {}

    TextureResource::~TextureResource() {
        cleanup();
    }

    bool TextureResource::onLoad(const FileData& data) {
        if (data.empty()) {
            return false;
        }

        try {
            loadFromImageData(data.getData(), data.size());
            loaded = true;
            return true;
        } catch (const std::exception&) {
            cleanup();
            loaded = false;
            return false;
        }
    }

    void TextureResource::onUnload() {
        cleanup();
        loaded = false;
    }

    void TextureResource::loadFromImageData(const uint8_t* data, size_t size) {
        // Create SDL_RWops from memory
        SDL_RWops* rw = SDL_RWFromConstMem(data, static_cast<int>(size));
        if (!rw) {
            throw std::runtime_error("Failed to create SDL_RWops from memory");
        }

        // Load image from memory
        SDL_Surface* surface = IMG_Load_RW(rw, 1); // 1 means free RW automatically
        if (!surface) {
            throw std::runtime_error("Failed to load image from memory: " + std::string(IMG_GetError()));
        }

        // Store dimensions
        width = surface->w;
        height = surface->h;

        // Determine format
        int colorDepth = surface->format->BytesPerPixel;
        if (colorDepth == 4) {
            format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
        } else {
            format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
        }

        // Generate and bind texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Free surface
        SDL_FreeSurface(surface);

        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cleanup();
            throw std::runtime_error("OpenGL error during texture creation: " + std::to_string(error));
        }
    }

    void TextureResource::cleanup() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
        width = 0;
        height = 0;
        format = GL_RGB;
    }

    void TextureResource::bind(unsigned int unit) const {
        if (loaded && textureID != 0) {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, textureID);
        }
    }

    void TextureResource::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    TextureResource::TextureResource(TextureResource&& other) noexcept
        : textureID(other.textureID), width(other.width), height(other.height),
        format(other.format), loaded(other.loaded) {
        other.textureID = 0;
        other.width = 0;
        other.height = 0;
        other.format = GL_RGB;
        other.loaded = false;
    }

    TextureResource& TextureResource::operator=(TextureResource&& other) noexcept {
        if (this != &other) {
            cleanup();

            textureID = other.textureID;
            width = other.width;
            height = other.height;
            format = other.format;
            loaded = other.loaded;

            other.textureID = 0;
            other.width = 0;
            other.height = 0;
            other.format = GL_RGB;
            other.loaded = false;
        }
        return *this;
    }

} // namespace resources

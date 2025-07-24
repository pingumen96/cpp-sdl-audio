#pragma once

#include "../IResource.h"
#include <GL/glew.h>
#include <string>

namespace resources {

    /**
     * @brief Texture resource that implements IResource interface
     */
    class TextureResource : public IResource {
    private:
        GLuint textureID;
        int width, height;
        GLenum format;
        bool loaded;

        void loadFromImageData(const uint8_t* data, size_t size);
        void cleanup();

    public:
        TextureResource();
        ~TextureResource() override;

        // IResource interface implementation
        bool onLoad(const FileData& data) override;
        void onUnload() override;
        bool isLoaded() const override { return loaded; }
        std::string getResourceType() const override { return "Texture"; }

        // Texture-specific methods
        GLuint getTextureID() const { return textureID; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        GLenum getFormat() const { return format; }

        void bind(unsigned int unit = 0) const;
        void unbind() const;

        // Prevent copy
        TextureResource(const TextureResource&) = delete;
        TextureResource& operator=(const TextureResource&) = delete;

        // Allow move
        TextureResource(TextureResource&& other) noexcept;
        TextureResource& operator=(TextureResource&& other) noexcept;
    };

} // namespace resources

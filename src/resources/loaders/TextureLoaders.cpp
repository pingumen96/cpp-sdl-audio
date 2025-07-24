#include "TextureLoaders.h"
#include "../LoaderFactory.h"
#include <algorithm>

namespace resources {

    // PngTextureLoader implementation
    bool PngTextureLoader::canLoad(const std::string& path) const {
        std::string ext = LoaderFactory::getFileExtension(path);
        return ext == ".png";
    }

    ResourcePtr PngTextureLoader::createEmpty() const {
        return std::make_shared<TextureResource>();
    }

    ResourcePtr PngTextureLoader::load(const std::string& path, const IFileSystem& fileSystem) const {
        if (!canLoad(path) || !fileSystem.exists(path)) {
            return nullptr;
        }

        FileData data = fileSystem.readAll(path);
        if (data.empty()) {
            return nullptr;
        }

        auto texture = std::make_shared<TextureResource>();
        if (texture->onLoad(data)) {
            return texture;
        }

        return nullptr;
    }

    std::vector<std::string> PngTextureLoader::getSupportedExtensions() const {
        return { ".png" };
    }

    std::string PngTextureLoader::getLoaderType() const {
        return "PngTextureLoader";
    }

    // ImageTextureLoader implementation
    bool ImageTextureLoader::canLoad(const std::string& path) const {
        std::string ext = LoaderFactory::getFileExtension(path);
        const auto& supportedExts = getSupportedExtensions();
        return std::find(supportedExts.begin(), supportedExts.end(), ext) != supportedExts.end();
    }

    ResourcePtr ImageTextureLoader::createEmpty() const {
        return std::make_shared<TextureResource>();
    }

    ResourcePtr ImageTextureLoader::load(const std::string& path, const IFileSystem& fileSystem) const {
        if (!canLoad(path) || !fileSystem.exists(path)) {
            return nullptr;
        }

        FileData data = fileSystem.readAll(path);
        if (data.empty()) {
            return nullptr;
        }

        auto texture = std::make_shared<TextureResource>();
        if (texture->onLoad(data)) {
            return texture;
        }

        return nullptr;
    }

    std::vector<std::string> ImageTextureLoader::getSupportedExtensions() const {
        return { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif" };
    }

    std::string ImageTextureLoader::getLoaderType() const {
        return "ImageTextureLoader";
    }

} // namespace resources

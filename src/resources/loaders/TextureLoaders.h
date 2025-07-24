#pragma once

#include "../IResourceLoader.h"
#include "../types/TextureResource.h"

namespace resources {

    /**
     * @brief Loader for PNG texture files
     */
    class PngTextureLoader : public IResourceLoader {
    public:
        bool canLoad(const std::string& path) const override;
        ResourcePtr createEmpty() const override;
        ResourcePtr load(const std::string& path, const IFileSystem& fileSystem) const override;
        std::vector<std::string> getSupportedExtensions() const override;
        std::string getLoaderType() const override;
    };

    /**
     * @brief Loader for common image formats (PNG, JPG, BMP, etc.)
     */
    class ImageTextureLoader : public IResourceLoader {
    public:
        bool canLoad(const std::string& path) const override;
        ResourcePtr createEmpty() const override;
        ResourcePtr load(const std::string& path, const IFileSystem& fileSystem) const override;
        std::vector<std::string> getSupportedExtensions() const override;
        std::string getLoaderType() const override;
    };

} // namespace resources

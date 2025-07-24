#include "LoaderFactory.h"
#include <filesystem>

namespace resources {

    void LoaderFactory::registerLoader(LoaderPtr loader) {
        if (loader) {
            loaders.push_back(std::move(loader));
        }
    }

    const IResourceLoader* LoaderFactory::findLoader(const std::string& path) const {
        for (const auto& loader : loaders) {
            if (loader->canLoad(path)) {
                return loader.get();
            }
        }
        return nullptr;
    }

    void LoaderFactory::clear() {
        loaders.clear();
    }

    std::string LoaderFactory::getFileExtension(const std::string& path) {
        try {
            std::filesystem::path filePath(path);
            std::string extension = filePath.extension().string();

            // Convert to lowercase for consistency
            std::transform(extension.begin(), extension.end(), extension.begin(),
                [](char c) { return std::tolower(c); });

            return extension;
        } catch (const std::exception&) {
            return "";
        }
    }

    bool LoaderFactory::isExtensionSupported(const std::string& extension) const {
        std::string lowerExt = extension;
        std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(),
            [](char c) { return std::tolower(c); });

        for (const auto& loader : loaders) {
            const auto& supportedExts = loader->getSupportedExtensions();
            for (const auto& ext : supportedExts) {
                std::string lowerSupportedExt = ext;
                std::transform(lowerSupportedExt.begin(), lowerSupportedExt.end(),
                    lowerSupportedExt.begin(), [](char c) { return std::tolower(c); });

                if (lowerExt == lowerSupportedExt) {
                    return true;
                }
            }
        }
        return false;
    }

} // namespace resources

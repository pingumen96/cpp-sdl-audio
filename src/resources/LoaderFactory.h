#pragma once

#include "IResourceLoader.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

namespace resources {

    /**
     * @brief Factory class for managing and selecting appropriate resource loaders
     */
    class LoaderFactory {
    private:
        std::vector<LoaderPtr> loaders;

    public:
        LoaderFactory() = default;
        ~LoaderFactory() = default;

        // Non-copyable but movable
        LoaderFactory(const LoaderFactory&) = delete;
        LoaderFactory& operator=(const LoaderFactory&) = delete;
        LoaderFactory(LoaderFactory&&) = default;
        LoaderFactory& operator=(LoaderFactory&&) = default;

        /**
         * @brief Register a new resource loader
         * @param loader Unique pointer to the loader to register
         */
        void registerLoader(LoaderPtr loader);

        /**
         * @brief Find appropriate loader for given file path
         * @param path File path to find loader for
         * @return Pointer to suitable loader or nullptr if none found
         */
        const IResourceLoader* findLoader(const std::string& path) const;

        /**
         * @brief Get all registered loaders as raw pointers (no ownership exposed)
         * @return Vector of raw pointers to loaders
         */
        std::vector<const IResourceLoader*> getLoaders() const {
            std::vector<const IResourceLoader*> result;
            result.reserve(loaders.size());
            for (const auto& loader : loaders) {
                result.push_back(loader.get());
            }
            return result;
        }

        /**
         * @brief Get number of registered loaders
         * @return Number of loaders
         */
        size_t getLoaderCount() const { return loaders.size(); }

        /**
         * @brief Clear all registered loaders
         */
        void clear();

        /**
         * @brief Get file extension from path (including the dot)
         * @param path File path
         * @return Extension string (e.g., ".png") or empty string if no extension
         */
        static std::string getFileExtension(const std::string& path);

        /**
         * @brief Check if extension is supported by any registered loader
         * @param extension File extension to check (including the dot)
         * @return true if supported, false otherwise
         */
        bool isExtensionSupported(const std::string& extension) const;
    };

} // namespace resources

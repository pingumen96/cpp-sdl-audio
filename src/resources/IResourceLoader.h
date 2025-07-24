#pragma once

#include "IResource.h"
#include "IFileSystem.h"
#include <string>
#include <memory>

namespace resources {

    /**
     * @brief Interface for specialized resource loaders
     */
    class IResourceLoader {
    public:
        virtual ~IResourceLoader() = default;

        /**
         * @brief Check if this loader can handle the given file path
         * @param path File path to check
         * @return true if this loader can handle the file, false otherwise
         */
        virtual bool canLoad(const std::string& path) const = 0;

        /**
         * @brief Create an empty resource instance of the appropriate type
         * @return Shared pointer to empty resource instance
         */
        virtual ResourcePtr createEmpty() const = 0;

        /**
         * @brief Load resource from file using provided file system
         * @param path File path to load
         * @param fileSystem File system interface to use for loading
         * @return Loaded resource or nullptr if loading failed
         */
        virtual ResourcePtr load(const std::string& path, const IFileSystem& fileSystem) const = 0;

        /**
         * @brief Get supported file extensions (including the dot, e.g., ".png")
         * @return Vector of supported extensions
         */
        virtual std::vector<std::string> getSupportedExtensions() const = 0;

        /**
         * @brief Get loader type name for debugging
         * @return Loader type name
         */
        virtual std::string getLoaderType() const = 0;
    };

    using LoaderPtr = std::unique_ptr<IResourceLoader>;

} // namespace resources

#pragma once

#include "IFileSystem.h"
#include "IResource.h"
#include "LoaderFactory.h"
#include "ResourceRegistry.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace resources {

    /**
     * @brief Resource loading options
     */
    struct LoadOptions {
        bool forceReload = false;       ///< Force reload even if already loaded
        bool async = false;             ///< Load asynchronously (not implemented in this version)
        size_t priority = 0;            ///< Loading priority (higher = more important)

        LoadOptions() = default;
        LoadOptions(bool forceReload) : forceReload(forceReload) {}
    };

    /**
     * @brief Resource loading result
     */
    enum class LoadResult {
        Success,            ///< Resource loaded successfully
        AlreadyLoaded,     ///< Resource was already loaded
        LoaderNotFound,    ///< No suitable loader found
        FileNotFound,      ///< File doesn't exist
        LoadingFailed,     ///< Loading process failed
        InvalidPath        ///< Invalid file path
    };

    /**
     * @brief Main facade class for resource management
     *
     * This class provides the public API for loading, accessing, and managing resources.
     * It coordinates between the file system, loaders, and registry to provide a
     * simple interface for resource operations.
     */
    class ResourceManager {
    private:
        std::unique_ptr<IFileSystem> fileSystem;
        LoaderFactory loaderFactory;
        ResourceRegistry registry;

        // Statistics
        size_t totalLoadsAttempted;
        size_t totalLoadsSuccessful;

    public:
        /**
         * @brief Constructor with custom file system
         * @param fileSystem Custom file system implementation
         */
        explicit ResourceManager(std::unique_ptr<IFileSystem> fileSystem);

        /**
         * @brief Constructor with default file system
         */
        ResourceManager();

        ~ResourceManager() = default;

        // Non-copyable but movable
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = default;
        ResourceManager& operator=(ResourceManager&&) = default;

        /**
         * @brief Register a resource loader
         * @param loader Unique pointer to the loader
         */
        void registerLoader(LoaderPtr loader);

        /**
         * @brief Load a resource from file path
         * @param path File path to load
         * @param options Loading options
         * @return Loading result status
         */
        LoadResult load(const std::string& path, const LoadOptions& options = LoadOptions{});

        /**
         * @brief Get a loaded resource by path, cast to specific type
         * @tparam T Resource type to cast to
         * @param path File path of the resource
         * @return Shared pointer to resource or nullptr if not found/wrong type
         */
        template<typename T>
        std::shared_ptr<T> get(const std::string& path);

        /**
         * @brief Get a loaded resource by GUID, cast to specific type
         * @tparam T Resource type to cast to
         * @param guid Resource GUID
         * @return Shared pointer to resource or nullptr if not found/wrong type
         */
        template<typename T>
        std::shared_ptr<T> getByGuid(const ResourceGUID& guid);

        /**
         * @brief Release reference to a resource
         * @param path File path of the resource
         * @return true if reference was released, false if resource not found
         */
        bool release(const std::string& path);

        /**
         * @brief Release reference to a resource by GUID
         * @param guid Resource GUID
         * @return true if reference was released, false if resource not found
         */
        bool releaseByGuid(const ResourceGUID& guid);

        /**
         * @brief Unload all unused resources (reference count = 0)
         * @return Number of resources unloaded
         */
        size_t unloadUnused();

        /**
         * @brief Force unload a specific resource
         * @param path File path of the resource
         * @return true if unloaded, false if not found
         */
        bool forceUnload(const std::string& path);

        /**
         * @brief Unload all resources
         */
        void unloadAll();

        /**
         * @brief Check if a resource is loaded
         * @param path File path to check
         * @return true if loaded, false otherwise
         */
        bool isLoaded(const std::string& path) const;

        /**
         * @brief Get resource loading state
         * @param path File path to check
         * @return Resource state
         */
        ResourceState getResourceState(const std::string& path) const;

        /**
         * @brief Get total memory usage of all loaded resources
         * @return Memory usage in bytes
         */
        size_t getTotalMemoryUsage() const;

        /**
         * @brief Get number of loaded resources
         * @return Resource count
         */
        size_t getLoadedResourceCount() const;

        /**
         * @brief Get loading statistics
         * @return Pair of (attempted loads, successful loads)
         */
        std::pair<size_t, size_t> getLoadingStats() const;

        /**
         * @brief Generate GUID for path
         * @param path File path
         * @return Generated GUID
         */
        ResourceGUID generateGUID(const std::string& path) const;

        /**
         * @brief Get all loaded resource paths
         * @return Vector of file paths
         */
        std::vector<std::string> getLoadedResourcePaths() const;

        /**
         * @brief Set custom file system
         * @param fileSystem New file system implementation
         */
        void setFileSystem(std::unique_ptr<IFileSystem> fileSystem);

        /**
         * @brief Get reference to the file system
         * @return Reference to current file system
         */
        const IFileSystem& getFileSystem() const;

        /**
         * @brief Get reference to the loader factory
         * @return Reference to loader factory
         */
        const LoaderFactory& getLoaderFactory() const;
    };

    // Template implementations
    template<typename T>
    std::shared_ptr<T> ResourceManager::get(const std::string& path) {
        ResourceGUID guid = generateGUID(path);
        return getByGuid<T>(guid);
    }

    template<typename T>
    std::shared_ptr<T> ResourceManager::getByGuid(const ResourceGUID& guid) {
        const ResourceRecord* record = registry.getRecord(guid);
        if (record && record->resource && record->state == ResourceState::Loaded) {
            // Add reference when accessing resource
            registry.addReference(guid);
            return std::dynamic_pointer_cast<T>(record->resource);
        }
        return nullptr;
    }

} // namespace resources

#pragma once

#include "IResource.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace resources {

    /**
     * @brief Resource loading states
     */
    enum class ResourceState {
        Unloaded,   ///< Resource not loaded
        Loading,    ///< Resource currently being loaded
        Loaded,     ///< Resource successfully loaded
        Failed      ///< Resource failed to load
    };

    /**
     * @brief GUID type for resource identification
     */
    using ResourceGUID = std::string;

    /**
     * @brief Record containing resource metadata and state
     */
    struct ResourceRecord {
        ResourcePtr resource;           ///< Pointer to the actual resource
        ResourceGUID guid;             ///< Unique identifier
        std::string path;              ///< Original file path
        ResourceState state;           ///< Current loading state
        int refCount;                  ///< Reference counter
        size_t memoryUsage;           ///< Estimated memory usage in bytes

        ResourceRecord()
            : state(ResourceState::Unloaded), refCount(0), memoryUsage(0) {}

        ResourceRecord(const ResourceGUID& guid, const std::string& path)
            : guid(guid), path(path), state(ResourceState::Unloaded),
            refCount(0), memoryUsage(0) {}
    };

    /**
     * @brief Central registry for managing resource records
     */
    class ResourceRegistry {
    private:
        std::unordered_map<ResourceGUID, ResourceRecord> resources;
        size_t totalMemoryUsage;

    public:
        ResourceRegistry() : totalMemoryUsage(0) {}
        ~ResourceRegistry() = default;

        // Non-copyable but movable
        ResourceRegistry(const ResourceRegistry&) = delete;
        ResourceRegistry& operator=(const ResourceRegistry&) = delete;
        ResourceRegistry(ResourceRegistry&&) = default;
        ResourceRegistry& operator=(ResourceRegistry&&) = default;

        /**
         * @brief Register a new resource record
         * @param guid Unique identifier for the resource
         * @param path File path of the resource
         * @return Reference to the created record
         */
        ResourceRecord& registerResource(const ResourceGUID& guid, const std::string& path);

        /**
         * @brief Get resource record by GUID
         * @param guid Resource identifier
         * @return Pointer to record or nullptr if not found
         */
        ResourceRecord* getRecord(const ResourceGUID& guid);

        /**
         * @brief Get resource record by GUID (const version)
         * @param guid Resource identifier
         * @return Pointer to record or nullptr if not found
         */
        const ResourceRecord* getRecord(const ResourceGUID& guid) const;

        /**
         * @brief Check if resource exists in registry
         * @param guid Resource identifier
         * @return true if exists, false otherwise
         */
        bool hasResource(const ResourceGUID& guid) const;

        /**
         * @brief Remove resource from registry
         * @param guid Resource identifier
         * @return true if removed, false if not found
         */
        bool unregisterResource(const ResourceGUID& guid);

        /**
         * @brief Increment reference count for resource
         * @param guid Resource identifier
         * @return New reference count or -1 if resource not found
         */
        int addReference(const ResourceGUID& guid);

        /**
         * @brief Decrement reference count for resource
         * @param guid Resource identifier
         * @return New reference count or -1 if resource not found
         */
        int removeReference(const ResourceGUID& guid);

        /**
         * @brief Get all resources with zero reference count
         * @return Vector of GUIDs for unused resources
         */
        std::vector<ResourceGUID> getUnusedResources() const;

        /**
         * @brief Get all registered resource GUIDs
         * @return Vector of all GUIDs
         */
        std::vector<ResourceGUID> getAllResourceGUIDs() const;

        /**
         * @brief Update memory usage for a resource
         * @param guid Resource identifier
         * @param newUsage New memory usage in bytes
         */
        void updateMemoryUsage(const ResourceGUID& guid, size_t newUsage);

        /**
         * @brief Get total memory usage of all resources
         * @return Total memory usage in bytes
         */
        size_t getTotalMemoryUsage() const { return totalMemoryUsage; }

        /**
         * @brief Get number of registered resources
         * @return Resource count
         */
        size_t getResourceCount() const { return resources.size(); }

        /**
         * @brief Clear all resources from registry
         */
        void clear();

        /**
         * @brief Generate a GUID from file path
         * @param path File path
         * @return Generated GUID
         */
        static ResourceGUID generateGUID(const std::string& path);
    };

} // namespace resources

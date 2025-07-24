#include "ResourceManager.h"

namespace resources {

    ResourceManager::ResourceManager(std::unique_ptr<IFileSystem> fileSystem)
        : fileSystem(std::move(fileSystem)), totalLoadsAttempted(0), totalLoadsSuccessful(0) {}

    ResourceManager::ResourceManager()
        : ResourceManager(std::make_unique<StandardFileSystem>()) {}

    void ResourceManager::registerLoader(LoaderPtr loader) {
        loaderFactory.registerLoader(std::move(loader));
    }

    LoadResult ResourceManager::load(const std::string& path, const LoadOptions& options) {
        totalLoadsAttempted++;

        if (path.empty()) {
            return LoadResult::InvalidPath;
        }

        // Normalize the path
        std::string normalizedPath = fileSystem->normalize(path);
        ResourceGUID guid = generateGUID(normalizedPath);

        // Check if already loaded and not forcing reload
        if (!options.forceReload && registry.hasResource(guid)) {
            const ResourceRecord* record = registry.getRecord(guid);
            if (record && record->state == ResourceState::Loaded) {
                registry.addReference(guid);
                totalLoadsSuccessful++;
                return LoadResult::AlreadyLoaded;
            }
        }

        // Check if file exists
        if (!fileSystem->exists(normalizedPath)) {
            return LoadResult::FileNotFound;
        }

        // Find appropriate loader
        const IResourceLoader* loader = loaderFactory.findLoader(normalizedPath);
        if (!loader) {
            return LoadResult::LoaderNotFound;
        }

        // Register or get existing record
        ResourceRecord& record = registry.registerResource(guid, normalizedPath);
        record.state = ResourceState::Loading;

        // Attempt to load the resource
        try {
            ResourcePtr resource = loader->load(normalizedPath, *fileSystem);
            if (resource && resource->isLoaded()) {
                record.resource = resource;
                record.state = ResourceState::Loaded;
                registry.addReference(guid);

                // Update memory usage (basic estimation)
                // In a real implementation, it would be better to have a more accurate way to estimate memory usage
                record.memoryUsage = fileSystem->size(normalizedPath);
                registry.updateMemoryUsage(guid, record.memoryUsage);

                totalLoadsSuccessful++;
                return LoadResult::Success;
            } else {
                record.state = ResourceState::Failed;
                return LoadResult::LoadingFailed;
            }
        } catch (const std::exception&) {
            record.state = ResourceState::Failed;
            return LoadResult::LoadingFailed;
        }
    }

    bool ResourceManager::release(const std::string& path) {
        ResourceGUID guid = generateGUID(path);
        return releaseByGuid(guid);
    }

    bool ResourceManager::releaseByGuid(const ResourceGUID& guid) {
        int newRefCount = registry.removeReference(guid);
        return newRefCount >= 0;
    }

    size_t ResourceManager::unloadUnused() {
        std::vector<ResourceGUID> unused = registry.getUnusedResources();
        size_t unloadedCount = 0;

        for (const ResourceGUID& guid : unused) {
            ResourceRecord* record = registry.getRecord(guid);
            if (record && record->resource) {
                record->resource->onUnload();
                record->resource.reset();
                record->state = ResourceState::Unloaded;
                unloadedCount++;
            }
        }

        return unloadedCount;
    }

    bool ResourceManager::forceUnload(const std::string& path) {
        ResourceGUID guid = generateGUID(path);
        ResourceRecord* record = registry.getRecord(guid);

        if (record && record->resource) {
            record->resource->onUnload();
            record->resource.reset();
            record->state = ResourceState::Unloaded;
            record->refCount = 0;
            return true;
        }

        return false;
    }

    void ResourceManager::unloadAll() {
        std::vector<ResourceGUID> allGuids = registry.getAllResourceGUIDs();

        for (const ResourceGUID& guid : allGuids) {
            ResourceRecord* record = registry.getRecord(guid);
            if (record && record->resource) {
                record->resource->onUnload();
                record->resource.reset();
                record->state = ResourceState::Unloaded;
                record->refCount = 0;
            }
        }
    }

    bool ResourceManager::isLoaded(const std::string& path) const {
        ResourceGUID guid = generateGUID(path);
        const ResourceRecord* record = registry.getRecord(guid);
        return record && record->state == ResourceState::Loaded;
    }

    ResourceState ResourceManager::getResourceState(const std::string& path) const {
        ResourceGUID guid = generateGUID(path);
        const ResourceRecord* record = registry.getRecord(guid);
        return record ? record->state : ResourceState::Unloaded;
    }

    size_t ResourceManager::getTotalMemoryUsage() const {
        return registry.getTotalMemoryUsage();
    }

    size_t ResourceManager::getLoadedResourceCount() const {
        return registry.getResourceCount();
    }

    std::pair<size_t, size_t> ResourceManager::getLoadingStats() const {
        return { totalLoadsAttempted, totalLoadsSuccessful };
    }

    ResourceGUID ResourceManager::generateGUID(const std::string& path) const {
        return ResourceRegistry::generateGUID(fileSystem->normalize(path));
    }

    std::vector<std::string> ResourceManager::getLoadedResourcePaths() const {
        std::vector<std::string> paths;
        std::vector<ResourceGUID> guids = registry.getAllResourceGUIDs();

        for (const ResourceGUID& guid : guids) {
            const ResourceRecord* record = registry.getRecord(guid);
            if (record && record->state == ResourceState::Loaded) {
                paths.push_back(record->path);
            }
        }

        return paths;
    }

    void ResourceManager::setFileSystem(std::unique_ptr<IFileSystem> fileSystem) {
        this->fileSystem = std::move(fileSystem);
    }

    const IFileSystem& ResourceManager::getFileSystem() const {
        return *fileSystem;
    }

    const LoaderFactory& ResourceManager::getLoaderFactory() const {
        return loaderFactory;
    }

} // namespace resources

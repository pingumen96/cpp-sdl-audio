#include "ResourceRegistry.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace resources {

    ResourceRecord& ResourceRegistry::registerResource(const ResourceGUID& guid, const std::string& path) {
        auto [it, inserted] = resources.emplace(guid, ResourceRecord(guid, path));
        return it->second;
    }

    ResourceRecord* ResourceRegistry::getRecord(const ResourceGUID& guid) {
        auto it = resources.find(guid);
        return (it != resources.end()) ? &it->second : nullptr;
    }

    const ResourceRecord* ResourceRegistry::getRecord(const ResourceGUID& guid) const {
        auto it = resources.find(guid);
        return (it != resources.end()) ? &it->second : nullptr;
    }

    bool ResourceRegistry::hasResource(const ResourceGUID& guid) const {
        return resources.find(guid) != resources.end();
    }

    bool ResourceRegistry::unregisterResource(const ResourceGUID& guid) {
        auto it = resources.find(guid);
        if (it != resources.end()) {
            totalMemoryUsage -= it->second.memoryUsage;
            resources.erase(it);
            return true;
        }
        return false;
    }

    int ResourceRegistry::addReference(const ResourceGUID& guid) {
        auto it = resources.find(guid);
        if (it != resources.end()) {
            return ++it->second.refCount;
        }
        return -1;
    }

    int ResourceRegistry::removeReference(const ResourceGUID& guid) {
        auto it = resources.find(guid);
        if (it != resources.end()) {
            it->second.refCount = std::max(0, it->second.refCount - 1);
            return it->second.refCount;
        }
        return -1;
    }

    std::vector<ResourceGUID> ResourceRegistry::getUnusedResources() const {
        std::vector<ResourceGUID> unused;
        for (const auto& [guid, record] : resources) {
            if (record.refCount == 0 && record.state == ResourceState::Loaded) {
                unused.push_back(guid);
            }
        }
        return unused;
    }

    std::vector<ResourceGUID> ResourceRegistry::getAllResourceGUIDs() const {
        std::vector<ResourceGUID> guids;
        guids.reserve(resources.size());
        for (const auto& [guid, record] : resources) {
            guids.push_back(guid);
        }
        return guids;
    }

    void ResourceRegistry::updateMemoryUsage(const ResourceGUID& guid, size_t newUsage) {
        auto it = resources.find(guid);
        if (it != resources.end()) {
            totalMemoryUsage -= it->second.memoryUsage;
            it->second.memoryUsage = newUsage;
            totalMemoryUsage += newUsage;
        }
    }

    void ResourceRegistry::clear() {
        resources.clear();
        totalMemoryUsage = 0;
    }

    ResourceGUID ResourceRegistry::generateGUID(const std::string& path) {
        // Simple hash-based GUID generation
        // In a production system, it would be better to use a more robust GUID generation method
        std::hash<std::string> hasher;
        size_t hash = hasher(path);

        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }

} // namespace resources

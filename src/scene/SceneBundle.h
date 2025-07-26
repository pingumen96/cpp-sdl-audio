#pragma once

#include "../resources/ResourceSystem.h"
#include <vector>
#include <string>
#include <unordered_set>

namespace scene {

    /**
     * @brief Bundle containing resource metadata for a scene
     *
     * SceneBundle describes the resources needed by a scene but doesn't
     * implement loading itself. It uses the ResourceManager API for
     * acquiring and releasing resources.
     */
    class SceneBundle {
    private:
        std::vector<std::string> resourcePaths;
        std::unordered_set<resources::ResourceGUID> resourceGuids;

        // Progress tracking
        mutable size_t loadedCount = 0;
        mutable bool progressDirty = true;

    public:
        /**
         * @brief Default constructor
         */
        SceneBundle() = default;

        /**
         * @brief Constructor with initial resource paths
         */
        SceneBundle(const std::vector<std::string>& paths) : resourcePaths(paths) {}

        /**
         * @brief Add a resource by path
         */
        void addResource(const std::string& path) {
            resourcePaths.push_back(path);
            progressDirty = true;
        }

        /**
         * @brief Add multiple resources
         */
        void addResources(const std::vector<std::string>& paths) {
            resourcePaths.insert(resourcePaths.end(), paths.begin(), paths.end());
            progressDirty = true;
        }

        /**
         * @brief Clear all resources
         */
        void clear() {
            resourcePaths.clear();
            resourceGuids.clear();
            loadedCount = 0;
            progressDirty = true;
        }

        /**
         * @brief Get all resource paths
         */
        const std::vector<std::string>& getResourcePaths() const {
            return resourcePaths;
        }

        /**
         * @brief Check if bundle is empty
         */
        bool isEmpty() const {
            return resourcePaths.empty();
        }

        /**
         * @brief Get total resource count
         */
        size_t getResourceCount() const {
            return resourcePaths.size();
        }

        /**
         * @brief Start loading all resources asynchronously
         * @param resourceManager ResourceManager instance to use
         * @param async Whether to load asynchronously
         * @return true if all resources started loading successfully
         */
        bool acquire(resources::ResourceManager& resourceManager, bool async = true) {
            resourceGuids.clear();
            bool allStarted = true;

            for (const auto& path : resourcePaths) {
                resources::LoadOptions options;
                options.async = async;

                auto result = resourceManager.load(path, options);
                if (result == resources::LoadResult::Success ||
                    result == resources::LoadResult::AlreadyLoaded) {

                    resources::ResourceGUID guid = resourceManager.generateGUID(path);
                    resourceGuids.insert(guid);
                } else {
                    allStarted = false;
                }
            }

            progressDirty = true;
            return allStarted;
        }

        /**
         * @brief Release all resources from the bundle
         * @param resourceManager ResourceManager instance to use
         * @return Number of resources successfully released
         */
        size_t release(resources::ResourceManager& resourceManager) {
            size_t releasedCount = 0;

            for (const auto& path : resourcePaths) {
                if (resourceManager.release(path)) {
                    releasedCount++;
                }
            }

            resourceGuids.clear();
            loadedCount = 0;
            progressDirty = true;

            return releasedCount;
        }

        /**
         * @brief Check if all resources are loaded and ready
         * @param resourceManager ResourceManager instance to check against
         * @return true if all resources are loaded
         */
        bool isReady(const resources::ResourceManager& resourceManager) const {
            for (const auto& path : resourcePaths) {
                if (!resourceManager.isLoaded(path)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Get loading progress (0.0 to 1.0)
         * @param resourceManager ResourceManager instance to check against
         * @return Progress ratio
         */
        float getProgress(const resources::ResourceManager& resourceManager) const {
            if (resourcePaths.empty()) {
                return 1.0f;
            }

            if (progressDirty) {
                loadedCount = 0;
                for (const auto& path : resourcePaths) {
                    if (resourceManager.isLoaded(path)) {
                        loadedCount++;
                    }
                }
                progressDirty = false;
            }

            return static_cast<float>(loadedCount) / static_cast<float>(resourcePaths.size());
        }

        /**
         * @brief Get a specific resource by path and type
         * @tparam T Resource type
         * @param resourceManager ResourceManager instance
         * @param path Resource path
         * @return Shared pointer to resource or nullptr if not found/loaded
         */
        template<typename T>
        std::shared_ptr<T> getResource(resources::ResourceManager& resourceManager,
            const std::string& path) const {
            return resourceManager.get<T>(path);
        }
    };

} // namespace scene

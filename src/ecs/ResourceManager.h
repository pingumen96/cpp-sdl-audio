#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <any>

namespace ecs {

    /**
     * @brief Manager for global resources in the ECS
     *
     * Resources are singleton-like objects that can be accessed by any system.
     * Unlike components, resources are not attached to entities.
     */
    class ResourceManager {
    private:
        std::unordered_map<std::type_index, std::any> mResources;

    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        // Non-copyable, non-movable
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        /**
         * @brief Add a resource of type T
         * @tparam T Resource type
         * @tparam Args Constructor argument types
         * @param args Arguments to construct the resource
         * @return Reference to the created resource
         */
        template<typename T, typename... Args>
        T& addResource(Args&&... args) {
            auto typeIndex = std::type_index(typeid(T));
            auto resource = std::make_unique<T>(std::forward<Args>(args)...);
            T* resourcePtr = resource.get();

            mResources[typeIndex] = std::move(resource);
            return *resourcePtr;
        }

        /**
         * @brief Get a resource of type T
         * @tparam T Resource type
         * @return Reference to the resource
         * @throws std::runtime_error if resource doesn't exist
         */
        template<typename T>
        T& getResource() {
            auto typeIndex = std::type_index(typeid(T));
            auto it = mResources.find(typeIndex);

            if (it == mResources.end()) {
                throw std::runtime_error("Resource not found");
            }

            try {
                auto& resourcePtr = std::any_cast<std::unique_ptr<T>&>(it->second);
                return *resourcePtr;
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Resource type mismatch");
            }
        }

        /**
         * @brief Get a resource of type T (const version)
         * @tparam T Resource type
         * @return Const reference to the resource
         * @throws std::runtime_error if resource doesn't exist
         */
        template<typename T>
        const T& getResource() const {
            auto typeIndex = std::type_index(typeid(T));
            auto it = mResources.find(typeIndex);

            if (it == mResources.end()) {
                throw std::runtime_error("Resource not found");
            }

            try {
                const auto& resourcePtr = std::any_cast<const std::unique_ptr<T>&>(it->second);
                return *resourcePtr;
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Resource type mismatch");
            }
        }

        /**
         * @brief Check if a resource of type T exists
         * @tparam T Resource type
         * @return true if resource exists, false otherwise
         */
        template<typename T>
        bool hasResource() const {
            auto typeIndex = std::type_index(typeid(T));
            return mResources.find(typeIndex) != mResources.end();
        }

        /**
         * @brief Remove a resource of type T
         * @tparam T Resource type
         * @return true if resource was removed, false if it didn't exist
         */
        template<typename T>
        bool removeResource() {
            auto typeIndex = std::type_index(typeid(T));
            return mResources.erase(typeIndex) > 0;
        }

        /**
         * @brief Clear all resources
         */
        void clear() {
            mResources.clear();
        }

        /**
         * @brief Get the number of registered resources
         * @return Resource count
         */
        size_t getResourceCount() const {
            return mResources.size();
        }
    };

} // namespace ecs

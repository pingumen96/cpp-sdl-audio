#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>

namespace ecs {

    /**
     * @brief Manager for runtime ECS resources
     *
     * This is different from the resources::ResourceManager which manages file-based resources.
     * This manages runtime global state objects like InputState, EventBus, etc.
     * 
     * Renamed from GlobalResourceHolder to better reflect its purpose and avoid confusion
     * with the file-based ResourceManager in the resources namespace.
     */
    class RuntimeResourceManager {
    private:
        std::unordered_map<std::type_index, std::shared_ptr<void>> mResources;

    public:
        RuntimeResourceManager() = default;
        ~RuntimeResourceManager() = default;

        // Non-copyable, non-movable
        RuntimeResourceManager(const RuntimeResourceManager&) = delete;
        RuntimeResourceManager& operator=(const RuntimeResourceManager&) = delete;
        RuntimeResourceManager(RuntimeResourceManager&&) = delete;
        RuntimeResourceManager& operator=(RuntimeResourceManager&&) = delete;

        /**
         * @brief Add a runtime resource of type T
         * @tparam T Resource type
         * @tparam Args Constructor argument types
         * @param args Arguments to construct the resource
         * @return Reference to the created resource
         */
        template<typename T, typename... Args>
        T& addResource(Args&&... args) {
            auto typeIndex = std::type_index(typeid(T));
            auto resource = std::make_shared<T>(std::forward<Args>(args)...);

            mResources[typeIndex] = std::static_pointer_cast<void>(resource);
            return *resource;
        }

        /**
         * @brief Get a runtime resource of type T
         * @tparam T Resource type
         * @return Reference to the resource
         * @throws std::runtime_error if resource doesn't exist
         */
        template<typename T>
        T& getResource() {
            auto typeIndex = std::type_index(typeid(T));
            auto it = mResources.find(typeIndex);

            if (it == mResources.end()) {
                throw std::runtime_error("Runtime resource not found");
            }

            auto resource = std::static_pointer_cast<T>(it->second);
            return *resource;
        }

        /**
         * @brief Get a runtime resource of type T (const version)
         * @tparam T Resource type
         * @return Const reference to the resource
         * @throws std::runtime_error if resource doesn't exist
         */
        template<typename T>
        const T& getResource() const {
            auto typeIndex = std::type_index(typeid(T));
            auto it = mResources.find(typeIndex);

            if (it == mResources.end()) {
                throw std::runtime_error("Runtime resource not found");
            }

            auto resource = std::static_pointer_cast<T>(it->second);
            return *resource;
        }

        /**
         * @brief Get a pointer to runtime resource of type T
         * @tparam T Resource type
         * @return Pointer to the resource or nullptr if not found
         */
        template<typename T>
        T* getResourcePtr() {
            auto typeIndex = std::type_index(typeid(T));
            auto it = mResources.find(typeIndex);

            if (it == mResources.end()) {
                return nullptr;
            }

            auto resource = std::static_pointer_cast<T>(it->second);
            return resource.get();
        }

        /**
         * @brief Check if a runtime resource of type T exists
         * @tparam T Resource type
         * @return true if resource exists, false otherwise
         */
        template<typename T>
        bool hasResource() const {
            auto typeIndex = std::type_index(typeid(T));
            return mResources.find(typeIndex) != mResources.end();
        }

        /**
         * @brief Remove a runtime resource of type T
         * @tparam T Resource type
         * @return true if resource was removed, false if it didn't exist
         */
        template<typename T>
        bool removeResource() {
            auto typeIndex = std::type_index(typeid(T));
            return mResources.erase(typeIndex) > 0;
        }

        /**
         * @brief Clear all runtime resources
         */
        void clear() {
            mResources.clear();
        }

        /**
         * @brief Get the number of registered runtime resources
         * @return Resource count
         */
        size_t getResourceCount() const {
            return mResources.size();
        }
    };

} // namespace ecs

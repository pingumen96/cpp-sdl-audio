#pragma once

#include "../ecs/ECS.h"
#include "../ecs/components/CommonComponents.h"
#include <unordered_map>
#include <typeindex>
#include <mutex>

namespace scene {

    /**
     * @brief Global registry for component types shared across all scene coordinators
     *
     * This registry ensures that component type IDs are consistent across all
     * scene coordinators while keeping their data completely isolated.
     */
    class ComponentTypeRegistry {
    private:
        static std::unordered_map<std::type_index, ecs::ComponentType> globalTypes;
        static ecs::ComponentType nextTypeId;
        static bool initialized;
        static std::mutex registryMutex;

        /**
         * @brief Internal version of getOrRegisterType that assumes mutex is already locked
         * @tparam T Component type
         * @return Consistent type ID across all coordinators
         */
        template<typename T>
        static ecs::ComponentType getOrRegisterTypeInternal() {
            std::type_index typeIndex = std::type_index(typeid(T));

            auto it = globalTypes.find(typeIndex);
            if (it != globalTypes.end()) {
                return it->second;
            }

            // Register new type with next available ID
            ecs::ComponentType newTypeId = nextTypeId++;
            globalTypes[typeIndex] = newTypeId;
            return newTypeId;
        }

    public:
        /**
         * @brief Get or register a component type
         * @tparam T Component type
         * @return Consistent type ID across all coordinators
         */
        template<typename T>
        static ecs::ComponentType getOrRegisterType() {
            std::lock_guard<std::mutex> lock(registryMutex);
            return getOrRegisterTypeInternal<T>();
        }

        /**
         * @brief Initialize all common component types used by scenes
         * This should be called once at startup
         */
        static void initializeCommonTypes() {
            std::lock_guard<std::mutex> lock(registryMutex);

            if (initialized) return;

            // Pre-register all common component types using internal version
            getOrRegisterTypeInternal<ecs::components::Transform>();
            getOrRegisterTypeInternal<ecs::components::Velocity>();
            getOrRegisterTypeInternal<ecs::components::Renderable>();
            getOrRegisterTypeInternal<ecs::components::Health>();
            getOrRegisterTypeInternal<ecs::components::PlayerTag>();
            getOrRegisterTypeInternal<ecs::components::EnemyTag>();

            initialized = true;
        }

        /**
         * @brief Check if component type is registered
         */
        template<typename T>
        static bool isTypeRegistered() {
            std::lock_guard<std::mutex> lock(registryMutex);

            std::type_index typeIndex = std::type_index(typeid(T));
            return globalTypes.find(typeIndex) != globalTypes.end();
        }

        /**
         * @brief Get registered type count
         */
        static size_t getRegisteredTypeCount() {
            std::lock_guard<std::mutex> lock(registryMutex);
            return globalTypes.size();
        }

        /**
         * @brief Reset registry (for testing)
         */
        static void reset() {
            std::lock_guard<std::mutex> lock(registryMutex);
            globalTypes.clear();
            nextTypeId = 0;
            initialized = false;
        }
    };

    // Static member definitions (to be put in .cpp file)
    inline std::unordered_map<std::type_index, ecs::ComponentType> ComponentTypeRegistry::globalTypes;
    inline ecs::ComponentType ComponentTypeRegistry::nextTypeId = 0;
    inline bool ComponentTypeRegistry::initialized = false;
    inline std::mutex ComponentTypeRegistry::registryMutex;

} // namespace scene

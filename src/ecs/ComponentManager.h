#pragma once

#include "IComponentArray.h"
#include "ComponentArray.h"
#include "ECSTypes.h"
#include <array>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>

// Forward declaration to avoid circular dependency
namespace scene {
    class ComponentTypeRegistry;
}

namespace ecs {

    /**
     * @brief Manages registration and storage of all component types
     *
     * Responsibilities:
     * - Register new component types and assign unique IDs
     * - Create and manage ComponentArray instances for each type
     * - Provide type-safe access to component data
     * - Handle entity destruction cleanup across all component types
     */
    class ComponentManager {
    private:
        /// Map from component type to unique component ID
        std::unordered_map<std::type_index, ComponentType> mComponentTypes;

        /// Array of component array pointers indexed by component type ID
        std::array<std::shared_ptr<IComponentArray>, MAX_COMPONENTS> mComponentArrays;

        /// Next available component type ID
        ComponentType mNextComponentType;

        /**
         * @brief Gets the component array for type T
         * @tparam T Component type
         * @return Pointer to ComponentArray<T>
         */
        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray() {
            std::type_index typeIndex = std::type_index(typeid(T));

            //std::cout << "[ComponentManager] Accessing component array for type: " << typeIndex.name() << std::endl;

            assert(mComponentTypes.find(typeIndex) != mComponentTypes.end() && "Component not registered before use.");

            return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[mComponentTypes[typeIndex]]);
        }

    public:
        /**
         * @brief Constructor
         */
        ComponentManager() : mNextComponentType(0) {}

        /**
         * @brief Helper function to get global component type (implemented in .cpp)
         */
        static ComponentType getGlobalComponentType(std::type_index typeIndex);

        /**
         * @brief Registers a new component type
         * @tparam T Component type to register
         */
        template<typename T>
        void registerComponent() {
            std::type_index typeIndex = std::type_index(typeid(T));

            // If component is already registered, return early (no error)
            if (mComponentTypes.find(typeIndex) != mComponentTypes.end()) {
                return;
            }

            // Get the global component type ID from registry
            // This ensures consistency across all coordinators
            ecs::ComponentType globalTypeId = getGlobalComponentType(typeIndex);

            // Add this component type to the component type map using global ID
            mComponentTypes[typeIndex] = globalTypeId;

            // Create a ComponentArray pointer and add it to the component arrays array
            mComponentArrays[globalTypeId] = std::make_shared<ComponentArray<T>>();

            // Update next component type to ensure we don't conflict
            if (globalTypeId >= mNextComponentType) {
                mNextComponentType = globalTypeId + 1;
            }
        }

        /**
         * @brief Gets the component type ID for a registered component type
         * @tparam T Component type
         * @return Component type ID
         */
        template<typename T>
        ComponentType getComponentType() {
            std::type_index typeIndex = std::type_index(typeid(T));

            //std::cout << "[ComponentManager] Getting component type ID for: " << typeIndex.name() << std::endl;

            assert(mComponentTypes.find(typeIndex) != mComponentTypes.end() && "Component not registered before use.");

            return mComponentTypes[typeIndex];
        }

        /**
         * @brief Adds a component to an entity
         * @tparam T Component type
         * @param entity Entity to add component to
         * @param component Component data
         */
        template<typename T>
        void addComponent(Entity entity, const T& component) {
            getComponentArray<T>()->insertData(entity, component);
        }

        /**
         * @brief Removes a component from an entity
         * @tparam T Component type
         * @param entity Entity to remove component from
         */
        template<typename T>
        void removeComponent(Entity entity) {
            getComponentArray<T>()->removeData(entity);
        }

        /**
         * @brief Gets a component from an entity
         * @tparam T Component type
         * @param entity Entity to get component from
         * @return Reference to the component
         */
        template<typename T>
        T& getComponent(Entity entity) {
            return getComponentArray<T>()->getData(entity);
        }

        /**
         * @brief Gets a component from an entity (const version)
         * @tparam T Component type
         * @param entity Entity to get component from
         * @return Const reference to the component
         */
        template<typename T>
        const T& getComponent(Entity entity) const {
            return const_cast<ComponentManager*>(this)->getComponentArray<T>()->getData(entity);
        }

        /**
         * @brief Checks if an entity has a specific component
         * @tparam T Component type
         * @param entity Entity to check
         * @return true if entity has component, false otherwise
         */
        template<typename T>
        bool hasComponent(Entity entity) const {
            return const_cast<ComponentManager*>(this)->getComponentArray<T>()->hasData(entity);
        }

        /**
         * @brief Notifies all component arrays that an entity has been destroyed
         * @param entity The destroyed entity
         */
        void entityDestroyed(Entity entity) {
            // Notify each component array that an entity has been destroyed
            // If it has a component for that entity, it will remove it
            for (auto const& componentArray : mComponentArrays) {
                if (componentArray) {
                    componentArray->entityDestroyed(entity);
                }
            }
        }

        /**
         * @brief Gets the number of registered component types
         * @return Number of component types
         */
        size_t getRegisteredComponentCount() const {
            return mComponentTypes.size();
        }

        /**
         * @brief Gets the current size of a specific component array
         * @tparam T Component type
         * @return Number of components of type T
         */
        template<typename T>
        size_t getComponentCount() const {
            return const_cast<ComponentManager*>(this)->getComponentArray<T>()->getSize();
        }
    };

} // namespace ecs

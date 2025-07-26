#pragma once

#include "IComponentArray.h"
#include "ECSTypes.h"
#include <array>
#include <unordered_map>
#include <cassert>

namespace ecs {

    /**
     * @brief Dense array storage for components of type T
     *
     * Maintains components in a contiguous array for cache efficiency.
     * Uses entity-to-index and index-to-entity mappings for O(1) access.
     *
     * @tparam T Component type to store
     */
    template<typename T>
    class ComponentArray : public IComponentArray {
    private:
        /// Dense array of components
        std::array<T, MAX_ENTITIES> mComponentArray;

        /// Map from entity ID to array index
        std::unordered_map<Entity, size_t> mEntityToIndexMap;

        /// Map from array index to entity ID (for efficient removal)
        std::unordered_map<size_t, Entity> mIndexToEntityMap;

        /// Current number of valid components in the array
        size_t mSize;

    public:
        /**
         * @brief Constructor
         */
        ComponentArray() : mSize(0) {}

        /**
         * @brief Adds a component for an entity
         * @param entity Entity to add component to
         * @param component Component data to add
         */
        void insertData(Entity entity, const T& component) {
            assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

            // Put new entry at end and update the maps
            size_t newIndex = mSize;
            mEntityToIndexMap[entity] = newIndex;
            mIndexToEntityMap[newIndex] = entity;
            mComponentArray[newIndex] = component;
            ++mSize;
        }

        /**
         * @brief Removes component for an entity using swap-and-pop
         * @param entity Entity to remove component from
         */
        void removeData(Entity entity) {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

            // Copy element at end into deleted element's place to maintain density
            size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
            size_t indexOfLastElement = mSize - 1;
            mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

            // Update map to point to moved spot
            Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
            mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            mEntityToIndexMap.erase(entity);
            mIndexToEntityMap.erase(indexOfLastElement);

            --mSize;
        }

        /**
         * @brief Gets component data for an entity
         * @param entity Entity to get component from
         * @return Reference to the component
         */
        T& getData(Entity entity) {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");
            return mComponentArray[mEntityToIndexMap[entity]];
        }

        /**
         * @brief Gets component data for an entity (const version)
         * @param entity Entity to get component from
         * @return Const reference to the component
         */
        const T& getData(Entity entity) const {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");
            return mComponentArray[mEntityToIndexMap.at(entity)];
        }

        /**
         * @brief Checks if an entity has this component
         * @param entity Entity to check
         * @return true if entity has component, false otherwise
         */
        bool hasData(Entity entity) const {
            return mEntityToIndexMap.find(entity) != mEntityToIndexMap.end();
        }

        /**
         * @brief Gets current number of components
         * @return Component count
         */
        size_t getSize() const {
            return mSize;
        }

        /**
         * @brief Called when an entity is destroyed - removes component if present
         * @param entity The entity that was destroyed
         */
        void entityDestroyed(Entity entity) override {
            if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
                removeData(entity);
            }
        }
    };

} // namespace ecs

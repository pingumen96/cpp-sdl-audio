#pragma once

#include "ECSTypes.h"
#include <vector>
#include <algorithm>

namespace ecs {

    /**
     * @brief Base class for all systems in the ECS
     *
     * Systems contain the logic that operates on entities with specific components.
     * Each system maintains a list of entities that match its required signature.
     */
    class System {
    protected:
        /// List of entities currently assigned to this system
        std::vector<Entity> mEntities;

    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~System() = default;

        /**
         * @brief Gets the list of entities managed by this system
         * @return Const reference to entity list
         */
        const std::vector<Entity>& getEntities() const {
            return mEntities;
        }

        /**
         * @brief Gets the number of entities managed by this system
         * @return Entity count
         */
        size_t getEntityCount() const {
            return mEntities.size();
        }

        /**
         * @brief Adds an entity to this system
         * @param entity Entity to add
         */
        void addEntity(Entity entity) {
            // Check if entity is already in the list
            auto it = std::find(mEntities.begin(), mEntities.end(), entity);
            if (it == mEntities.end()) {
                mEntities.push_back(entity);
            }
        }

        /**
         * @brief Removes an entity from this system
         * @param entity Entity to remove
         */
        void removeEntity(Entity entity) {
            auto it = std::find(mEntities.begin(), mEntities.end(), entity);
            if (it != mEntities.end()) {
                mEntities.erase(it);
            }
        }

        /**
         * @brief Checks if an entity is managed by this system
         * @param entity Entity to check
         * @return true if entity is in system, false otherwise
         */
        bool hasEntity(Entity entity) const {
            auto it = std::find(mEntities.begin(), mEntities.end(), entity);
            return it != mEntities.end();
        }

        /**
         * @brief System update method - override in derived classes
         * @param deltaTime Time elapsed since last update
         */
        virtual void update(float deltaTime) {
            // Default implementation does nothing
            // Override in derived classes to implement system logic
            (void)deltaTime; // Suppress unused parameter warning
        }
    };

} // namespace ecs

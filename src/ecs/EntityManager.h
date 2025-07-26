#pragma once

#include "ECSTypes.h"
#include <array>
#include <queue>
#include <cassert>

namespace ecs {

    /**
     * @brief Manages entity creation, destruction, and signatures
     *
     * Responsibilities:
     * - Provide unique entity IDs through ID recycling
     * - Maintain entity signatures (component bitsets)
     * - Track active entity count
     */
    class EntityManager {
    private:
        /// Queue of available entity IDs for recycling
        std::queue<Entity> mAvailableEntities;

        /// Array storing signature for each entity
        std::array<Signature, MAX_ENTITIES> mSignatures;

        /// Number of currently active entities
        size_t mLivingEntityCount;

    public:
        /**
         * @brief Constructor - initializes available entity queue
         */
        EntityManager() : mLivingEntityCount(0) {
            // Initialize all entity IDs as available
            for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
                mAvailableEntities.push(entity);
            }
        }

        /**
         * @brief Creates a new entity
         * @return Unique entity ID
         * @throws std::runtime_error if no entities are available
         */
        Entity createEntity() {
            assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

            // Take an ID from the front of the queue
            Entity id = mAvailableEntities.front();
            mAvailableEntities.pop();
            ++mLivingEntityCount;

            return id;
        }

        /**
         * @brief Destroys an entity and recycles its ID
         * @param entity Entity to destroy
         */
        void destroyEntity(Entity entity) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Invalidate the destroyed entity's signature
            mSignatures[entity].reset();

            // Put the destroyed ID at the back of the queue
            mAvailableEntities.push(entity);
            --mLivingEntityCount;
        }

        /**
         * @brief Sets the signature for an entity
         * @param entity Entity to modify
         * @param signature New signature to assign
         */
        void setSignature(Entity entity, const Signature& signature) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");
            mSignatures[entity] = signature;
        }

        /**
         * @brief Gets the signature for an entity
         * @param entity Entity to query
         * @return Current signature of the entity
         */
        Signature getSignature(Entity entity) const {
            assert(entity < MAX_ENTITIES && "Entity out of range.");
            return mSignatures[entity];
        }

        /**
         * @brief Gets the number of living entities
         * @return Current active entity count
         */
        size_t getLivingEntityCount() const {
            return mLivingEntityCount;
        }
    };

} // namespace ecs

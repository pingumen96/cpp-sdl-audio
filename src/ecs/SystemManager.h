#pragma once

#include "System.h"
#include "ECSTypes.h"
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>

namespace ecs {

    /**
     * @brief Manages system registration and entity assignment
     *
     * Responsibilities:
     * - Register systems and their required component signatures
     * - Maintain entity lists for each system based on their signatures
     * - Update system entity lists when entity signatures change
     * - Handle entity destruction cleanup across all systems
     */
    class SystemManager {
    private:
        /// Map from system type to required component signature
        std::unordered_map<std::type_index, Signature> mSignatures;

        /// Map from system type to system instance
        std::unordered_map<std::type_index, std::shared_ptr<System>> mSystems;

    public:
        /**
         * @brief Registers a new system
         * @tparam T System type
         * @tparam Args Constructor argument types
         * @param args Constructor arguments
         * @return Shared pointer to the created system
         */
        template<typename T, typename... Args>
        std::shared_ptr<T> registerSystem(Args&&... args) {
            std::type_index typeIndex = std::type_index(typeid(T));

            assert(mSystems.find(typeIndex) == mSystems.end() && "Registering system more than once.");

            // Create a pointer to the system and return it so it can be used externally
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            mSystems[typeIndex] = system;
            return system;
        }

        /**
         * @brief Sets the component signature for a system
         * @tparam T System type
         * @param signature Required component signature
         */
        template<typename T>
        void setSignature(Signature signature) {
            std::type_index typeIndex = std::type_index(typeid(T));

            assert(mSystems.find(typeIndex) != mSystems.end() && "System used before registered.");

            // Set the signature for this system
            mSignatures[typeIndex] = signature;
        }

        /**
         * @brief Gets a registered system
         * @tparam T System type
         * @return Shared pointer to the system
         */
        template<typename T>
        std::shared_ptr<T> getSystem() {
            std::type_index typeIndex = std::type_index(typeid(T));

            assert(mSystems.find(typeIndex) != mSystems.end() && "System used before registered.");

            return std::static_pointer_cast<T>(mSystems[typeIndex]);
        }

        /**
         * @brief Removes an entity from all systems
         * @param entity The destroyed entity
         */
        void entityDestroyed(Entity entity) {
            // Erase a destroyed entity from all system lists
            for (auto const& pair : mSystems) {
                auto const& system = pair.second;
                system->removeEntity(entity);
            }
        }

        /**
         * @brief Updates system entity lists when an entity's signature changes
         * @param entity The entity whose signature changed
         * @param entitySignature The entity's current signature
         */
        void entitySignatureChanged(Entity entity, Signature entitySignature) {
            // Notify each system that an entity's signature changed
            for (auto const& pair : mSystems) {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = mSignatures[type];

                // Entity signature matches system signature - insert into set
                if ((entitySignature & systemSignature) == systemSignature) {
                    system->addEntity(entity);
                }
                // Entity signature does not match system signature - erase from set
                else {
                    system->removeEntity(entity);
                }
            }
        }

        /**
         * @brief Gets the number of registered systems
         * @return System count
         */
        size_t getSystemCount() const {
            return mSystems.size();
        }

        /**
         * @brief Updates all systems
         * @param deltaTime Time elapsed since last update
         */
        void updateAllSystems(float deltaTime) {
            for (auto const& pair : mSystems) {
                pair.second->update(deltaTime);
            }
        }

        /**
         * @brief Gets all registered systems
         * @return Map of system type indices to system instances
         */
        const std::unordered_map<std::type_index, std::shared_ptr<System>>& getSystems() const {
            return mSystems;
        }

        /**
         * @brief Gets the signature for a system type
         * @tparam T System type
         * @return System's required signature
         */
        template<typename T>
        Signature getSignature() const {
            std::type_index typeIndex = std::type_index(typeid(T));

            auto it = mSignatures.find(typeIndex);
            assert(it != mSignatures.end() && "System signature requested before being set.");

            return it->second;
        }
    };

} // namespace ecs

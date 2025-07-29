#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "RuntimeResourceManager.h"
#include "ECSTypes.h"
#include <memory>

namespace ecs {

    /**
     * @brief Main coordinator class providing unified access to ECS functionality
     *
     * The Coordinator acts as a facade that brings together EntityManager,
     * ComponentManager, and SystemManager to provide a simple, unified interface
     * for working with the Entity Component System.
     *
     * Responsibilities:
     * - Provide centralized access to all ECS operations
     * - Coordinate between managers to maintain system consistency
     * - Handle entity lifecycle management
     * - Manage component registration and manipulation
     * - Coordinate system registration and updates
     */
    class Coordinator {
    private:
        std::unique_ptr<EntityManager> mEntityManager;
        std::unique_ptr<ComponentManager> mComponentManager;
        std::unique_ptr<SystemManager> mSystemManager;
        std::unique_ptr<RuntimeResourceManager> mRuntimeResourceManager;

    public:
        /**
         * @brief Constructor - initializes all managers
         */
        Coordinator() {
            mEntityManager = std::make_unique<EntityManager>();
            mComponentManager = std::make_unique<ComponentManager>();
            mSystemManager = std::make_unique<SystemManager>();
            mRuntimeResourceManager = std::make_unique<RuntimeResourceManager>();
        }

        // Entity methods

        /**
         * @brief Creates a new entity
         * @return Unique entity ID
         */
        Entity createEntity() {
            return mEntityManager->createEntity();
        }

        /**
         * @brief Destroys an entity and cleans up all associated data
         * @param entity Entity to destroy
         */
        void destroyEntity(Entity entity) {
            mEntityManager->destroyEntity(entity);
            mComponentManager->entityDestroyed(entity);
            mSystemManager->entityDestroyed(entity);
        }

        /**
         * @brief Gets the number of living entities
         * @return Current active entity count
         */
        size_t getLivingEntityCount() const {
            return mEntityManager->getLivingEntityCount();
        }

        // Component methods

        /**
         * @brief Registers a new component type
         * @tparam T Component type to register
         */
        template<typename T>
        void registerComponent() {
            mComponentManager->registerComponent<T>();
        }

        /**
         * @brief Adds a component to an entity
         * @tparam T Component type
         * @param entity Entity to add component to
         * @param component Component data
         */
        template<typename T>
        void addComponent(Entity entity, const T& component) {
            mComponentManager->addComponent<T>(entity, component);

            auto signature = mEntityManager->getSignature(entity);
            signature.set(mComponentManager->getComponentType<T>(), true);
            mEntityManager->setSignature(entity, signature);

            mSystemManager->entitySignatureChanged(entity, signature);
        }

        /**
         * @brief Removes a component from an entity
         * @tparam T Component type
         * @param entity Entity to remove component from
         */
        template<typename T>
        void removeComponent(Entity entity) {
            mComponentManager->removeComponent<T>(entity);

            auto signature = mEntityManager->getSignature(entity);
            signature.set(mComponentManager->getComponentType<T>(), false);
            mEntityManager->setSignature(entity, signature);

            mSystemManager->entitySignatureChanged(entity, signature);
        }

        /**
         * @brief Gets a component from an entity
         * @tparam T Component type
         * @param entity Entity to get component from
         * @return Reference to the component
         */
        template<typename T>
        T& getComponent(Entity entity) {
            return mComponentManager->getComponent<T>(entity);
        }

        /**
         * @brief Gets a component from an entity (const version)
         * @tparam T Component type
         * @param entity Entity to get component from
         * @return Const reference to the component
         */
        template<typename T>
        const T& getComponent(Entity entity) const {
            return mComponentManager->getComponent<T>(entity);
        }

        /**
         * @brief Checks if an entity has a specific component
         * @tparam T Component type
         * @param entity Entity to check
         * @return true if entity has component, false otherwise
         */
        template<typename T>
        bool hasComponent(Entity entity) const {
            return mComponentManager->hasComponent<T>(entity);
        }

        /**
         * @brief Gets the component type ID for a registered component type
         * @tparam T Component type
         * @return Component type ID
         */
        template<typename T>
        ComponentType getComponentType() {
            return mComponentManager->getComponentType<T>();
        }

        /**
         * @brief Gets the current size of a specific component array
         * @tparam T Component type
         * @return Number of components of type T
         */
        template<typename T>
        size_t getComponentCount() const {
            return mComponentManager->getComponentCount<T>();
        }

        // System methods

        /**
         * @brief Registers a new system
         * @tparam T System type
         * @tparam Args Constructor argument types
         * @param args Constructor arguments
         * @return Shared pointer to the created system
         */
        template<typename T, typename... Args>
        std::shared_ptr<T> registerSystem(Args&&... args) {
            return mSystemManager->registerSystem<T>(std::forward<Args>(args)...);
        }

        /**
         * @brief Sets the component signature for a system
         * @tparam T System type
         * @param signature Required component signature
         */
        template<typename T>
        void setSystemSignature(Signature signature) {
            mSystemManager->setSignature<T>(signature);

            // Update system entity lists for all existing entities
            for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
                if (mEntityManager->getLivingEntityCount() == 0) break;

                auto entitySignature = mEntityManager->getSignature(entity);
                if (entitySignature.any()) { // Entity is alive if it has any signature
                    mSystemManager->entitySignatureChanged(entity, entitySignature);
                }
            }
        }

        /**
         * @brief Gets a registered system
         * @tparam T System type
         * @return Shared pointer to the system
         */
        template<typename T>
        std::shared_ptr<T> getSystem() {
            return mSystemManager->getSystem<T>();
        }

        /**
         * @brief Updates all systems
         * @param deltaTime Time elapsed since last update
         */
        void updateSystems(float deltaTime) {
            mSystemManager->updateAllSystems(deltaTime);
        }

        /**
         * @brief Gets the number of registered systems
         * @return System count
         */
        size_t getSystemCount() const {
            return mSystemManager->getSystemCount();
        }

        /**
         * @brief Gets the signature for an entity
         * @param entity Entity to query
         * @return Entity's current signature
         */
        Signature getEntitySignature(Entity entity) const {
            return mEntityManager->getSignature(entity);
        }

        /**
         * @brief Gets the signature for a system type
         * @tparam T System type
         * @return System's required signature
         */
        template<typename T>
        Signature getSystemSignature() const {
            return mSystemManager->getSignature<T>();
        }

        /**
         * @brief Gets the number of registered component types
         * @return Number of component types
         */
        size_t getRegisteredComponentCount() const {
            return mComponentManager->getRegisteredComponentCount();
        }

        // Runtime resource methods
        // These manage runtime objects in the ECS system (InputState, EventBus, etc.)
        // Different from resources::ResourceManager which handles file-based assets

        /**
         * @brief Add a runtime resource of type T
         * @tparam T Resource type
         * @tparam Args Constructor argument types
         * @param args Arguments to construct the resource
         * @return Reference to the created resource
         */
        template<typename T, typename... Args>
        T& addRuntimeResource(Args&&... args) {
            return mRuntimeResourceManager->addResource<T>(std::forward<Args>(args)...);
        }

        /**
         * @brief Get a runtime resource of type T
         * @tparam T Resource type
         * @return Reference to the resource
         * @throws std::runtime_error if resource doesn't exist
         */
        template<typename T>
        T& getRuntimeResource() {
            return mRuntimeResourceManager->getResource<T>();
        }

        /**
         * @brief Get a runtime resource of type T (const version)
         * @tparam T Resource type
         * @return Const reference to the resource
         * @throws std::runtime_error if resource doesn't exist
         */
        template<typename T>
        const T& getRuntimeResource() const {
            return mRuntimeResourceManager->getResource<T>();
        }

        /**
         * @brief Get a pointer to runtime resource of type T
         * @tparam T Resource type
         * @return Pointer to the resource or nullptr if not found
         */
        template<typename T>
        T* getRuntimeResourcePtr() {
            return mRuntimeResourceManager->getResourcePtr<T>();
        }

        /**
         * @brief Check if a runtime resource of type T exists
         * @tparam T Resource type
         * @return true if resource exists, false otherwise
         */
        template<typename T>
        bool hasRuntimeResource() const {
            return mRuntimeResourceManager->hasResource<T>();
        }

        /**
         * @brief Remove a runtime resource of type T
         * @tparam T Resource type
         * @return true if resource was removed, false if it didn't exist
         */
        template<typename T>
        bool removeRuntimeResource() {
            return mRuntimeResourceManager->removeResource<T>();
        }

        /**
         * @brief Get the number of registered runtime resources
         * @return Runtime resource count
         */
        size_t getRuntimeResourceCount() const {
            return mRuntimeResourceManager->getResourceCount();
        }
    };

} // namespace ecs

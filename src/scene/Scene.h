#pragma once

#include "SceneTypes.h"
#include "SceneBundle.h"
#include "SceneNode.h"
#include "rendering/RenderQueueBuilder.h"
#include "../ecs/ECS.h"
#include "../ecs/components/CommonComponents.h"
#include "../resources/ResourceSystem.h"
#include <memory>
#include <string>

namespace scene {

    // Forward declaration
    class SceneManager;

    /**
     * @brief Abstract base class for all scenes
     * 
     * A Scene encapsulates:
     * - An ECS world (identified by WorldID)
     * - A scene graph hierarchy (SceneNode)
     * - A resource bundle describing needed assets
     * 
     * The scene has a well-defined lifecycle:
     * onAttach → onActivate → update/render → onDeactivate → onDetach
     */
    class Scene {
    protected:
        WorldID worldId = INVALID_WORLD_ID;
        std::unique_ptr<ecs::Coordinator> coordinator;
        std::unique_ptr<SceneNode> rootNode;
        SceneBundle resourceBundle;
        std::string sceneName;
        bool paused = false;
        bool pausesUnderlying = true; // Whether this scene pauses underlying scenes

    public:
        /**
         * @brief Constructor
         * @param name Scene name for debugging
         */
        explicit Scene(const std::string& name = "Scene") 
            : sceneName(name), rootNode(std::make_unique<SceneNode>("Root")) {
            coordinator = ecs::createCoordinator();
            
            // Register common components used by scenes
            coordinator->registerComponent<ecs::components::Transform>();
            coordinator->registerComponent<ecs::components::Velocity>();
            coordinator->registerComponent<ecs::components::Renderable>();
            coordinator->registerComponent<ecs::components::Health>();
            coordinator->registerComponent<ecs::components::PlayerTag>();
            coordinator->registerComponent<ecs::components::EnemyTag>();
        }

        /**
         * @brief Virtual destructor
         */
        virtual ~Scene() = default;

        // Non-copyable but movable
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = default;
        Scene& operator=(Scene&&) = default;

        // Lifecycle methods (to be implemented by derived classes)

        /**
         * @brief Called when scene is attached to scene manager
         * This is where you should:
         * - Register ECS systems
         * - Build the scene hierarchy
         * - Set up the resource bundle
         * 
         * @param manager Reference to the scene manager
         */
        virtual void onAttach(SceneManager& manager) = 0;

        /**
         * @brief Called when scene becomes the active scene
         * This is where you should:
         * - Start systems that should only run when active
         * - Resume audio/animations
         */
        virtual void onActivate() {}

        /**
         * @brief Called when scene is no longer the active scene
         * This is where you should:
         * - Pause systems
         * - Stop audio/animations
         */
        virtual void onDeactivate() {}

        /**
         * @brief Called when scene is removed from scene manager
         * This is where you should:
         * - Clean up resources (handled automatically by bundle)
         * - Perform final cleanup
         * 
         * @param manager Reference to the scene manager
         */
        virtual void onDetach(SceneManager& manager) {
            // Default implementation releases resources
            if (auto* resourceManager = getResourceManager(manager)) {
                resourceBundle.release(*resourceManager);
            }
        }

        /**
         * @brief Update scene logic
         * @param deltaTime Time elapsed since last update
         */
        virtual void update(float deltaTime) {
            if (!paused && coordinator) {
                coordinator->updateSystems(deltaTime);
            }
        }

        /**
         * @brief Render scene
         * @param builder RenderQueueBuilder for submitting draw commands
         */
        virtual void render(RenderQueueBuilder& builder) = 0;

        // Public interface methods

        /**
         * @brief Get scene name
         */
        const std::string& getName() const { return sceneName; }

        /**
         * @brief Set scene name
         */
        void setName(const std::string& name) { sceneName = name; }

        /**
         * @brief Get world ID
         */
        WorldID getWorldId() const { return worldId; }

        /**
         * @brief Get ECS coordinator
         */
        ecs::Coordinator* getCoordinator() const { return coordinator.get(); }

        /**
         * @brief Get root scene node
         */
        SceneNode* getRootNode() const { return rootNode.get(); }

        /**
         * @brief Get resource bundle
         */
        const SceneBundle& getResourceBundle() const { return resourceBundle; }

        /**
         * @brief Get mutable resource bundle
         */
        SceneBundle& getResourceBundle() { return resourceBundle; }

        /**
         * @brief Check if scene is paused
         */
        bool isPaused() const { return paused; }

        /**
         * @brief Set scene pause state
         */
        void setPaused(bool isPaused) { paused = isPaused; }

        /**
         * @brief Check if this scene pauses underlying scenes
         */
        bool getPausesUnderlying() const { return pausesUnderlying; }

        /**
         * @brief Set whether this scene pauses underlying scenes
         */
        void setPausesUnderlying(bool pauses) { pausesUnderlying = pauses; }

        /**
         * @brief Check if resource bundle is ready
         * @param manager Scene manager to get resource manager from
         */
        bool isResourceBundleReady(SceneManager& manager) const;

        /**
         * @brief Get resource bundle loading progress
         * @param manager Scene manager to get resource manager from
         */
        float getResourceBundleProgress(SceneManager& manager) const;

    protected:
        /**
         * @brief Helper to get resource manager from scene manager
         * @param manager Scene manager reference
         * @return Pointer to resource manager or nullptr
         */
        resources::ResourceManager* getResourceManager(SceneManager& manager) const;

        /**
         * @brief Set world ID (called by scene manager)
         */
        void setWorldId(WorldID id) { worldId = id; }

        // Allow SceneManager to set world ID
        friend class SceneManager;
    };

} // namespace scene

#pragma once

#include "../scene/Scene2D.h"
#include "../ecs/InputState.h"
#include "../ecs/EventBus.h"
#include "../ecs/GlobalFlags.h"
#include "../ecs/systems/InputCollectSystem.h"
#include "../ecs/systems/InputMappingSystem.h"

namespace game {

    /**
     * @brief Base class for scenes that need ECS-based input management
     *
     * This class extends Scene2D and automatically sets up:
     * - InputState global resource
     * - EventBus global resource
     * - GlobalFlags global resource
     * - InputCollectSystem (runs first)
     * - InputMappingSystem (runs second)
     */
    class InputEnabledScene : public scene::Scene2D {
    protected:
        // System references for easy access
        std::shared_ptr<ecs::systems::InputCollectSystem> inputCollectSystem;
        std::shared_ptr<ecs::systems::InputMappingSystem> inputMappingSystem;

    public:
        InputEnabledScene(const std::string& name = "InputEnabledScene")
            : Scene2D() {
            setName(name);
        }

        void onAttach(scene::SceneManager& manager) override {
            // Initialize base 2D scene
            initialize2D(manager);

            // Set up ECS input resources and systems
            setupInputResources();
            setupInputSystems();
        }

        void update(float deltaTime) override {
            // Check if we should quit the game
            if (coordinator->hasGlobalResource<ecs::GlobalFlags>()) {
                auto& globalFlags = coordinator->getGlobalResource<ecs::GlobalFlags>();
                if (globalFlags.quit) {
                    // Handle quit - this could emit an event or call a callback
                    handleQuitRequest();
                }
            }

            // Update base scene (this will run all systems including input systems)
            Scene2D::update(deltaTime);
        }

        /**
         * @brief Get the input state resource
         */
        ecs::InputState* getInputState() {
            return coordinator->getGlobalResourcePtr<ecs::InputState>();
        }

        /**
         * @brief Get the event bus resource
         */
        ecs::EventBus* getEventBus() {
            return coordinator->getGlobalResourcePtr<ecs::EventBus>();
        }

        /**
         * @brief Get the global flags resource
         */
        ecs::GlobalFlags* getGlobalFlags() {
            return coordinator->getGlobalResourcePtr<ecs::GlobalFlags>();
        }

    protected:
        /**
         * @brief Set up input-related global resources
         */
        void setupInputResources() {
            // Add global resources to this scene's coordinator
            coordinator->addGlobalResource<ecs::InputState>();
            coordinator->addGlobalResource<ecs::EventBus>();
            coordinator->addGlobalResource<ecs::GlobalFlags>();
        }

        /**
         * @brief Set up input-related systems
         */
        void setupInputSystems() {
            std::cout << "[InputEnabledScene] Setting up input systems for scene: " << getName() << std::endl;

            // Register and initialize input collection system
            inputCollectSystem = coordinator->registerSystem<ecs::systems::InputCollectSystem>();
            inputCollectSystem->init(
                coordinator->getGlobalResourcePtr<ecs::InputState>(),
                coordinator->getGlobalResourcePtr<ecs::GlobalFlags>(),
                coordinator->getGlobalResourcePtr<ecs::EventBus>()
            );

            // Register and initialize input mapping system
            inputMappingSystem = coordinator->registerSystem<ecs::systems::InputMappingSystem>();
            inputMappingSystem->init(
                coordinator->getGlobalResourcePtr<ecs::InputState>(),
                coordinator->getGlobalResourcePtr<ecs::EventBus>()
            );

            // Set system signatures (no specific components required for input systems)
            ecs::Signature inputSignature;
            coordinator->setSystemSignature<ecs::systems::InputCollectSystem>(inputSignature);
            coordinator->setSystemSignature<ecs::systems::InputMappingSystem>(inputSignature);

            std::cout << "[InputEnabledScene] Input systems initialized successfully for scene: " << getName() << std::endl;
        }

        /**
         * @brief Handle quit request - can be overridden by derived classes
         */
        virtual void handleQuitRequest() {
            std::cout << "[InputEnabledScene] Quit requested from scene: " << getName() << std::endl;
            // Default implementation - derived classes should override this
        }

        /**
         * @brief Setup ECS systems - adds input systems to base setup
         */
        void setupECSSystems() override {
            // Call base setup
            Scene2D::setupECSSystems();

            // Add input systems - these are already set up in setupInputSystems()
            // but we ensure they're called in the right order here
            std::cout << "[InputEnabledScene] Input systems added to ECS pipeline for scene: " << getName() << std::endl;
        }
    };

} // namespace game

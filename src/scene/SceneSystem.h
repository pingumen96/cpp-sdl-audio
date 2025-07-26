#pragma once

/**
 * @file SceneSystem.h
 * @brief Main header for the Scene Management System
 *
 * This file includes all the necessary components for using the scene system.
 * Include this file to get access to all scene management functionality.
 */

 // Core types and definitions
#include "SceneTypes.h"

// Component type registry
#include "ComponentTypeRegistry.h"

// Components
#include "components/Transform.h"

// Scene hierarchy
#include "SceneNode.h"

// Resource management
#include "SceneBundle.h"

// Rendering system
#include "rendering/CommandBuffer.h"
#include "rendering/IRenderBackend.h"
#include "rendering/NullBackend.h"
#include "rendering/RenderQueueBuilder.h"

// Systems
#include "systems/TransformSyncSystem.h"

// Transitions
#include "SceneTransition.h"
#include "SceneTransitions.h"

// Core scene classes
#include "Scene.h"
#include "SceneManager.h"

// Example scenes
#include "examples/TestScenes.h"

namespace scene {

    /**
     * @brief Create a default scene manager with null backend
     * @param width Initial render width
     * @param height Initial render height
     * @return Configured SceneManager ready for use
     */
    inline std::unique_ptr<SceneManager> createDefaultSceneManager(uint32_t width = 1920, uint32_t height = 1080) {
        auto backend = std::make_unique<NullBackend>();
        auto manager = std::make_unique<SceneManager>(std::move(backend));

        if (!manager->initialize(width, height)) {
            return nullptr;
        }

        return manager;
    }

    /**
     * @brief Create a scene manager with custom backend
     * @param backend Custom render backend
     * @param width Initial render width
     * @param height Initial render height
     * @return Configured SceneManager or nullptr if initialization failed
     */
    inline std::unique_ptr<SceneManager> createSceneManager(RenderBackendPtr backend,
        uint32_t width = 1920,
        uint32_t height = 1080) {
        auto manager = std::make_unique<SceneManager>(std::move(backend));

        if (!manager->initialize(width, height)) {
            return nullptr;
        }

        return manager;
    }

    /**
     * @brief Create a fade transition with specified duration
     * @param duration Transition duration in seconds
     * @return Unique pointer to fade transition
     */
    inline std::unique_ptr<FadeTransition> createFadeTransition(float duration = 1.0f) {
        return std::make_unique<FadeTransition>(duration);
    }

    /**
     * @brief Create a loading transition with progress bar
     * @param minTime Minimum time to show loading screen
     * @param resourceManager Optional resource manager for monitoring progress
     * @return Unique pointer to loading transition
     */
    inline std::unique_ptr<LoadingTransition> createLoadingTransition(float minTime = 0.5f,
        resources::ResourceManager* resourceManager = nullptr) {
        return std::make_unique<LoadingTransition>(minTime, resourceManager);
    }

    /**
     * @brief Create an instant transition (no visual effect)
     * @return Unique pointer to instant transition
     */
    inline std::unique_ptr<InstantTransition> createInstantTransition() {
        return std::make_unique<InstantTransition>();
    }

} // namespace scene

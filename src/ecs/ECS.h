#pragma once

/**
 * @file ECS.h
 * @brief Main header for the Entity Component System
 *
 * This file includes all the necessary components for using the ECS.
 * Include this file to get access to all ECS functionality.
 */

 // Core types and constants
#include "ECSTypes.h"

// Core interfaces
#include "IComponentArray.h"

// Component management
#include "ComponentArray.h"
#include "ComponentManager.h"

// Entity management
#include "EntityManager.h"

// System management
#include "System.h"
#include "SystemManager.h"

// Main coordinator
#include "Coordinator.h"

namespace ecs {

    /**
     * @brief Creates a default ECS coordinator instance
     * @return Unique pointer to a new Coordinator
     */
    inline std::unique_ptr<Coordinator> createCoordinator() {
        return std::make_unique<Coordinator>();
    }

} // namespace ecs

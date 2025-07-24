#pragma once

/**
 * @file ResourceSystem.h
 * @brief Main header for the resource management system
 *
 * This file includes all the necessary components for using the resource management system.
 * Include this file to get access to all resource management functionality.
 */

 // Core interfaces
#include "IFileSystem.h"
#include "IResource.h"
#include "IResourceLoader.h"

// Core classes
#include "LoaderFactory.h"
#include "ResourceRegistry.h"
#include "ResourceManager.h"

// Resource types
#include "types/TextureResource.h"
#include "types/MeshResource.h"

// Loaders
#include "loaders/TextureLoaders.h"
#include "loaders/MeshLoaders.h"

namespace resources {

    /**
     * @brief Initialize a resource manager with default loaders
     * @return Configured ResourceManager with standard loaders registered
     */
    inline std::unique_ptr<ResourceManager> createDefaultResourceManager() {
        auto resourceManager = std::make_unique<ResourceManager>();

        // Register texture loaders
        resourceManager->registerLoader(std::make_unique<PngTextureLoader>());
        resourceManager->registerLoader(std::make_unique<ImageTextureLoader>());

        // Register mesh loaders
        resourceManager->registerLoader(std::make_unique<ObjMeshLoader>());

        return resourceManager;
    }

} // namespace resources

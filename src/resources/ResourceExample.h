#pragma once

/**
 * @file ResourceExample.h
 * @brief Example demonstrating how to use the resource management system
 */

#include "ResourceSystem.h"
#include <iostream>
#include <memory>

namespace resources {

    /**
     * @brief Example class showing how to use the resource management system
     */
    class ResourceExample {
    private:
        std::unique_ptr<ResourceManager> resourceManager;

    public:
        ResourceExample() {
            // Initialize resource manager with default loaders
            resourceManager = createDefaultResourceManager();
        }

        /**
         * @brief Demonstrate basic resource loading and usage
         */
        void demonstrateBasicUsage() {
            std::cout << "=== Resource Management System Demo ===" << std::endl;

            // Load a texture (assuming we have a test texture)
            std::string texturePath = "data/preview.png";
            LoadResult result = resourceManager->load(texturePath);

            switch (result) {
            case LoadResult::Success:
                std::cout << "✓ Texture loaded successfully!" << std::endl;
                break;
            case LoadResult::AlreadyLoaded:
                std::cout << "✓ Texture was already loaded!" << std::endl;
                break;
            case LoadResult::FileNotFound:
                std::cout << "✗ Texture file not found: " << texturePath << std::endl;
                break;
            case LoadResult::LoaderNotFound:
                std::cout << "✗ No suitable loader found for: " << texturePath << std::endl;
                break;
            case LoadResult::LoadingFailed:
                std::cout << "✗ Failed to load texture: " << texturePath << std::endl;
                break;
            default:
                std::cout << "✗ Unknown error loading texture" << std::endl;
                break;
            }

            // Try to get the loaded texture
            auto texture = resourceManager->get<TextureResource>(texturePath);
            if (texture) {
                std::cout << "✓ Retrieved texture resource successfully!" << std::endl;
                std::cout << "  - Size: " << texture->getWidth() << "x" << texture->getHeight() << std::endl;
                std::cout << "  - OpenGL ID: " << texture->getTextureID() << std::endl;
                std::cout << "  - Type: " << texture->getResourceType() << std::endl;
            } else {
                std::cout << "✗ Failed to retrieve texture resource" << std::endl;
            }

            // Display resource manager statistics
            displayStatistics();

            // Demonstrate resource cleanup
            demonstrateCleanup();
        }

        /**
         * @brief Demonstrate resource cleanup operations
         */
        void demonstrateCleanup() {
            std::cout << "\n=== Resource Cleanup Demo ===" << std::endl;

            // Show loaded resources
            auto loadedPaths = resourceManager->getLoadedResourcePaths();
            std::cout << "Loaded resources: " << loadedPaths.size() << std::endl;
            for (const auto& path : loadedPaths) {
                std::cout << "  - " << path << std::endl;
            }

            // Unload unused resources
            size_t unloadedCount = resourceManager->unloadUnused();
            std::cout << "Unloaded " << unloadedCount << " unused resources" << std::endl;

            // Display updated statistics
            displayStatistics();
        }

        /**
         * @brief Demonstrate advanced features
         */
        void demonstrateAdvancedFeatures() {
            std::cout << "\n=== Advanced Features Demo ===" << std::endl;

            // Load with custom options
            LoadOptions options;
            options.forceReload = true;

            std::string meshPath = "data/test_mesh.obj";
            LoadResult result = resourceManager->load(meshPath, options);

            if (result == LoadResult::Success || result == LoadResult::AlreadyLoaded) {
                auto mesh = resourceManager->get<MeshResource>(meshPath);
                if (mesh) {
                    std::cout << "✓ Retrieved mesh resource successfully!" << std::endl;
                    std::cout << "  - Vertices: " << mesh->getVertexCount() << std::endl;
                    std::cout << "  - Triangles: " << mesh->getTriangleCount() << std::endl;
                    std::cout << "  - Type: " << mesh->getResourceType() << std::endl;
                }
            } else {
                std::cout << "Note: Test mesh file not found - this is expected for the demo" << std::endl;
            }

            // Show loader information
            const LoaderFactory& factory = resourceManager->getLoaderFactory();
            std::cout << "Registered loaders: " << factory.getLoaderCount() << std::endl;

            for (const auto& loader : factory.getLoaders()) {
                std::cout << "  - " << loader->getLoaderType() << " (";
                const auto& extensions = loader->getSupportedExtensions();
                for (size_t i = 0; i < extensions.size(); ++i) {
                    std::cout << extensions[i];
                    if (i < extensions.size() - 1) std::cout << ", ";
                }
                std::cout << ")" << std::endl;
            }
        }

        /**
         * @brief Display resource manager statistics
         */
        void displayStatistics() {
            auto [attempted, successful] = resourceManager->getLoadingStats();

            std::cout << "\n--- Resource Manager Statistics ---" << std::endl;
            std::cout << "Loaded resources: " << resourceManager->getLoadedResourceCount() << std::endl;
            std::cout << "Total memory usage: " << resourceManager->getTotalMemoryUsage() << " bytes" << std::endl;
            std::cout << "Load attempts: " << attempted << std::endl;
            std::cout << "Successful loads: " << successful << std::endl;
            if (attempted > 0) {
                double successRate = (double)successful / attempted * 100.0;
                std::cout << "Success rate: " << successRate << "%" << std::endl;
            }
        }

        /**
         * @brief Run all demonstrations
         */
        void runAllDemos() {
            demonstrateBasicUsage();
            demonstrateAdvancedFeatures();

            std::cout << "\n=== Resource Management Demo Complete ===" << std::endl;
        }
    };

} // namespace resources

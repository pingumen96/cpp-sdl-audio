#include <catch2/catch_test_macros.hpp>
#include "../../resources/ResourceSystem.h"
#include "../../scene/SceneBundle.h"
#include <iostream>

using namespace resources;

TEST_CASE("Resource System Debug", "[resources]") {
    
    SECTION("Step 1: Create ResourceManager") {
        std::cout << "Creating ResourceManager..." << std::endl;
        
        auto resourceManager = createDefaultResourceManager();
        REQUIRE(resourceManager != nullptr);
        
        std::cout << "ResourceManager created successfully" << std::endl;
    }
    
    SECTION("Step 2: Check file existence") {
        auto resourceManager = createDefaultResourceManager();
        
        std::cout << "Checking if test files exist..." << std::endl;
        
        // Check files that TestScene is trying to load
        std::vector<std::string> testFiles = {
            "data/test.wav",
            "data/preview.png"
        };
        
        for (const auto& file : testFiles) {
            // Implementazione specifica per testare l'esistenza del file
            std::cout << "Checking file: " << file << std::endl;
            
            LoadResult result = resourceManager->load(file);
            std::cout << "Load result for '" << file << "': " << static_cast<int>(result) << std::endl;
            
            if (result == LoadResult::FileNotFound) {
                std::cout << "File not found: " << file << std::endl;
            } else if (result == LoadResult::LoaderNotFound) {
                std::cout << "No loader found for: " << file << std::endl;
            } else if (result == LoadResult::LoadingFailed) {
                std::cout << "Loading failed for: " << file << std::endl;
            } else if (result == LoadResult::Success || result == LoadResult::AlreadyLoaded) {
                std::cout << "Successfully loaded: " << file << std::endl;
            }
        }
    }
    
    SECTION("Step 3: Test SceneBundle acquire manually") {
        std::cout << "Testing SceneBundle acquire manually..." << std::endl;
        
        auto resourceManager = createDefaultResourceManager();
        
        // Create a SceneBundle manually like TestScene does
        scene::SceneBundle bundle;
        bundle.addResource("data/test.wav");
        bundle.addResource("data/preview.png");
        
        std::cout << "Bundle created with " << bundle.getResourceCount() << " resources" << std::endl;
        
        // Try to acquire - this is where the crash happens
        bool result;
        REQUIRE_NOTHROW([&]() {
            result = bundle.acquire(*resourceManager, true);
        }());
        
        std::cout << "Bundle acquire result: " << result << std::endl;
    }
}

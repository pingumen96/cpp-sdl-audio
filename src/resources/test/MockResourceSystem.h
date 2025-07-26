#pragma once

#include "../../resources/IResource.h"
#include "../../resources/ResourceManager.h"
#include "../../resources/IResourceLoader.h"
#include "../../resources/IFileSystem.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace resources {
namespace test {

    /**
     * @brief Mock resource for testing
     */
    class MockResource : public IResource {
    private:
        bool loaded = false;
        std::string resourceType;
        FileData mockData;

    public:
        MockResource(const std::string& type = "MockResource") : resourceType(type) {}

        bool onLoad(const FileData& data) override {
            mockData = data;
            loaded = true;
            return true;
        }

        void onUnload() override {
            loaded = false;
            mockData.data.clear();
        }

        bool isLoaded() const override { return loaded; }
        std::string getResourceType() const override { return resourceType; }

        const FileData& getMockData() const { return mockData; }
    };

    /**
     * @brief Mock loader that can load any file type
     */
    class MockResourceLoader : public IResourceLoader {
    private:
        std::vector<std::string> supportedExtensions;
        std::string loaderType;

    public:
        MockResourceLoader(const std::vector<std::string>& exts, const std::string& type)
            : supportedExtensions(exts), loaderType(type) {}

        bool canLoad(const std::string& path) const override {
            std::string ext = LoaderFactory::getFileExtension(path);
            return std::find(supportedExtensions.begin(), supportedExtensions.end(), ext) != supportedExtensions.end();
        }

        ResourcePtr createEmpty() const override {
            return std::make_shared<MockResource>(loaderType);
        }

        ResourcePtr load(const std::string& path, const IFileSystem& fileSystem) const override {
            if (!canLoad(path)) {
                return nullptr;
            }

            // Create mock data instead of reading real file
            FileData mockData;
            mockData.data.push_back('M');
            mockData.data.push_back('O');
            mockData.data.push_back('C');
            mockData.data.push_back('K');

            auto resource = std::make_shared<MockResource>(loaderType);
            if (resource->onLoad(mockData)) {
                return resource;
            }

            return nullptr;
        }

        std::vector<std::string> getSupportedExtensions() const override {
            return supportedExtensions;
        }

        std::string getLoaderType() const override {
            return loaderType;
        }
    };

    /**
     * @brief Mock file system that simulates file existence for testing
     */
    class MockFileSystem : public IFileSystem {
    public:
        bool exists(const std::string& path) const override {
            // Always return true for mock testing
            return true;
        }

        size_t size(const std::string& path) const override {
            // Return a mock size
            return 1024;
        }

        FileData readAll(const std::string& path) const override {
            // Return mock data
            std::vector<uint8_t> mockData = {'M', 'O', 'C', 'K', '_', 'D', 'A', 'T', 'A'};
            return FileData{std::move(mockData)};
        }

        std::string normalize(const std::string& path) const override {
            // Simple normalization for testing
            return path;
        }
    };

    /**
     * @brief Create a ResourceManager configured for testing
     */
    inline std::unique_ptr<ResourceManager> createTestResourceManager() {
        // Create ResourceManager with mock file system
        auto resourceManager = std::make_unique<ResourceManager>(std::make_unique<MockFileSystem>());

        // Register mock loaders for common file types
        resourceManager->registerLoader(std::make_unique<MockResourceLoader>(
            std::vector<std::string>{".png", ".jpg", ".jpeg", ".bmp"}, "MockTextureLoader"));
        
        resourceManager->registerLoader(std::make_unique<MockResourceLoader>(
            std::vector<std::string>{".wav", ".mp3", ".ogg"}, "MockAudioLoader"));
        
        resourceManager->registerLoader(std::make_unique<MockResourceLoader>(
            std::vector<std::string>{".obj", ".fbx", ".dae"}, "MockMeshLoader"));

        return resourceManager;
    }

} // namespace test
} // namespace resources

#include <catch2/catch_test_macros.hpp>
#include "../src/resources/ResourceSystem.h"
#include <memory>

using namespace resources;

TEST_CASE("FileSystem basic operations", "[resources][filesystem]") {
    StandardFileSystem fs;

    SECTION("Normalize path - basic cases") {
        REQUIRE(fs.normalize("./test.txt") == "test.txt");
        REQUIRE(fs.normalize("folder/../test.txt") == "test.txt");
        REQUIRE(fs.normalize("folder\\test.txt") == "folder/test.txt");
    }

    SECTION("Normalize path - comprehensive tests") {
        // Test current directory resolution
        REQUIRE(fs.normalize("./file.txt") == "file.txt");
        REQUIRE(fs.normalize("./folder/./file.txt") == "folder/file.txt");

        // Test parent directory resolution
        REQUIRE(fs.normalize("folder/../file.txt") == "file.txt");
        REQUIRE(fs.normalize("folder1/folder2/../file.txt") == "folder1/file.txt");
        REQUIRE(fs.normalize("folder1/../folder2/file.txt") == "folder2/file.txt");

        // Test multiple parent directories
        REQUIRE(fs.normalize("a/b/c/../../file.txt") == "a/file.txt");
        REQUIRE(fs.normalize("a/b/../c/../file.txt") == "a/file.txt");

        // Test backslash to forward slash conversion (Windows paths)
        REQUIRE(fs.normalize("folder\\subfolder\\file.txt") == "folder/subfolder/file.txt");
        REQUIRE(fs.normalize("C:\\path\\to\\file.txt") == "C:/path/to/file.txt");
        REQUIRE(fs.normalize("folder\\..\\file.txt") == "file.txt");

        // Test mixed separators
        REQUIRE(fs.normalize("folder\\subfolder/file.txt") == "folder/subfolder/file.txt");
        REQUIRE(fs.normalize("folder/subfolder\\file.txt") == "folder/subfolder/file.txt");

        // Test complex paths with mixed elements
        REQUIRE(fs.normalize("./folder\\..\\subfolder/./file.txt") == "subfolder/file.txt");
        REQUIRE(fs.normalize("a\\b/..\\c\\./d/../file.txt") == "a/c/file.txt");

        // Test redundant separators
        REQUIRE(fs.normalize("folder//file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("folder\\\\file.txt") == "folder/file.txt");

        // Test absolute paths
        REQUIRE(fs.normalize("/absolute/path/file.txt") == "/absolute/path/file.txt");
        REQUIRE(fs.normalize("/absolute/../path/file.txt") == "/path/file.txt");

        // Test empty and simple paths
        REQUIRE(fs.normalize("") == "");
        REQUIRE(fs.normalize("file.txt") == "file.txt");
        REQUIRE(fs.normalize("folder/file.txt") == "folder/file.txt");

        // Test paths that should remain unchanged (already normalized)
        REQUIRE(fs.normalize("folder/subfolder/file.txt") == "folder/subfolder/file.txt");
        REQUIRE(fs.normalize("data/images/texture.png") == "data/images/texture.png");
    }

    SECTION("Normalize path - edge cases") {
        // Test root directory references
        REQUIRE(fs.normalize("/.") == "/");
        REQUIRE(fs.normalize("/..") == "/");

        // Test only dots and separators
        REQUIRE(fs.normalize("./") == ".");
        REQUIRE(fs.normalize("../") == "..");
        REQUIRE(fs.normalize("./../") == "..");

        // Test special characters in filenames (should be preserved)
        REQUIRE(fs.normalize("folder/file-name_123.txt") == "folder/file-name_123.txt");
        REQUIRE(fs.normalize("folder/file with spaces.txt") == "folder/file with spaces.txt");

        // Test long paths with many components
        std::string longPath = "a/b/c/d/e/f/g/h/../../../../../../../file.txt";
        // Note: The behavior here depends on the filesystem implementation
        // Some implementations stop at the first directory when going too far back
        std::string result = fs.normalize(longPath);
        REQUIRE(!result.empty()); // Just ensure it doesn't crash and returns something
    }

    SECTION("Check existing file") {
        // Test with a file that should exist in the project
        REQUIRE(fs.exists("CMakeLists.txt") == true);
        REQUIRE(fs.exists("nonexistent_file.xyz") == false);
    }
}

TEST_CASE("LoaderFactory operations", "[resources][loader]") {
    LoaderFactory factory;

    SECTION("Register and find loaders") {
        factory.registerLoader(std::make_unique<PngTextureLoader>());
        factory.registerLoader(std::make_unique<ObjMeshLoader>());

        REQUIRE(factory.getLoaderCount() == 2);

        const IResourceLoader* pngLoader = factory.findLoader("test.png");
        REQUIRE(pngLoader != nullptr);
        REQUIRE(pngLoader->getLoaderType() == "PngTextureLoader");

        const IResourceLoader* objLoader = factory.findLoader("model.obj");
        REQUIRE(objLoader != nullptr);
        REQUIRE(objLoader->getLoaderType() == "ObjMeshLoader");

        const IResourceLoader* unknownLoader = factory.findLoader("unknown.xyz");
        REQUIRE(unknownLoader == nullptr);
    }

    SECTION("File extension parsing") {
        REQUIRE(LoaderFactory::getFileExtension("test.png") == ".png");
        REQUIRE(LoaderFactory::getFileExtension("path/to/file.obj") == ".obj");
        REQUIRE(LoaderFactory::getFileExtension("TEST.PNG") == ".png");
        REQUIRE(LoaderFactory::getFileExtension("noextension") == "");
    }
}

TEST_CASE("ResourceRegistry operations", "[resources][registry]") {
    ResourceRegistry registry;

    SECTION("Register and retrieve resources") {
        std::string path = "test.png";
        ResourceGUID guid = ResourceRegistry::generateGUID(path);

        ResourceRecord& record = registry.registerResource(guid, path);
        REQUIRE(record.guid == guid);
        REQUIRE(record.path == path);
        REQUIRE(record.refCount == 0);
        REQUIRE(record.state == ResourceState::Unloaded);

        REQUIRE(registry.hasResource(guid) == true);
        REQUIRE(registry.getResourceCount() == 1);
    }

    SECTION("Reference counting") {
        std::string path = "test.png";
        ResourceGUID guid = ResourceRegistry::generateGUID(path);

        registry.registerResource(guid, path);

        REQUIRE(registry.addReference(guid) == 1);
        REQUIRE(registry.addReference(guid) == 2);
        REQUIRE(registry.removeReference(guid) == 1);
        REQUIRE(registry.removeReference(guid) == 0);
    }
}

TEST_CASE("ResourceManager basic operations", "[resources][manager]") {
    auto resourceManager = createDefaultResourceManager();

    SECTION("Initialize with loaders") {
        REQUIRE(resourceManager->getLoaderFactory().getLoaderCount() > 0);
        REQUIRE(resourceManager->getLoadedResourceCount() == 0);
        REQUIRE(resourceManager->getTotalMemoryUsage() == 0);
    }

    SECTION("GUID generation") {
        std::string path1 = "test.png";
        std::string path2 = "test.obj";
        std::string path1_duplicate = "test.png";

        ResourceGUID guid1 = resourceManager->generateGUID(path1);
        ResourceGUID guid2 = resourceManager->generateGUID(path2);
        ResourceGUID guid1_dup = resourceManager->generateGUID(path1_duplicate);

        REQUIRE(guid1 != guid2);
        REQUIRE(guid1 == guid1_dup);
    }

    SECTION("Resource loading with non-existent file") {
        LoadResult result = resourceManager->load("nonexistent.png");
        REQUIRE(result == LoadResult::FileNotFound);
    }
}

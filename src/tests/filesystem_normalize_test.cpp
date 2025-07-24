#include <catch2/catch_test_macros.hpp>
#include "../resources/IFileSystem.h"

using namespace resources;

TEST_CASE("StandardFileSystem normalize method", "[filesystem][normalize]") {
    StandardFileSystem fs;

    SECTION("Current directory resolution") {
        // Test single current directory reference
        REQUIRE(fs.normalize("./file.txt") == "file.txt");
        REQUIRE(fs.normalize("./") == ".");

        // Test nested current directory references
        REQUIRE(fs.normalize("./folder/./file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("./folder/./subfolder/./file.txt") == "folder/subfolder/file.txt");

        // Test current directory at different positions
        REQUIRE(fs.normalize("folder/./file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("folder/subfolder/./file.txt") == "folder/subfolder/file.txt");
    }

    SECTION("Parent directory resolution") {
        // Test single parent directory reference
        REQUIRE(fs.normalize("folder/../file.txt") == "file.txt");
        REQUIRE(fs.normalize("../file.txt") == "../file.txt");

        // Test nested parent directory references
        REQUIRE(fs.normalize("folder1/folder2/../file.txt") == "folder1/file.txt");
        REQUIRE(fs.normalize("folder1/../folder2/file.txt") == "folder2/file.txt");

        // Test multiple consecutive parent directories
        REQUIRE(fs.normalize("a/b/c/../../file.txt") == "a/file.txt");
        REQUIRE(fs.normalize("a/b/c/../../../file.txt") == "file.txt");

        // Test complex parent directory navigation
        REQUIRE(fs.normalize("a/b/../c/../d/file.txt") == "a/d/file.txt");
        REQUIRE(fs.normalize("a/b/c/../../d/../e/file.txt") == "a/e/file.txt");
    }

    SECTION("Backslash to forward slash conversion") {
        // Test simple backslash conversion
        REQUIRE(fs.normalize("folder\\file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("folder\\subfolder\\file.txt") == "folder/subfolder/file.txt");

        // Test Windows-style absolute paths
        REQUIRE(fs.normalize("C:\\Windows\\System32\\file.dll") == "C:/Windows/System32/file.dll");
        REQUIRE(fs.normalize("D:\\Projects\\MyProject\\src\\main.cpp") == "D:/Projects/MyProject/src/main.cpp");

        // Test backslashes with directory navigation
        REQUIRE(fs.normalize("folder\\..\\file.txt") == "file.txt");
        REQUIRE(fs.normalize("folder\\subfolder\\..\\file.txt") == "folder/file.txt");

        // Test mixed current and parent directory with backslashes
        REQUIRE(fs.normalize("folder\\.\\..\\file.txt") == "file.txt");
        REQUIRE(fs.normalize(".\\folder\\..\\file.txt") == "file.txt");
    }

    SECTION("Mixed path separators") {
        // Test forward slash followed by backslash
        REQUIRE(fs.normalize("folder/subfolder\\file.txt") == "folder/subfolder/file.txt");
        REQUIRE(fs.normalize("folder\\subfolder/file.txt") == "folder/subfolder/file.txt");

        // Test complex mixed separators with navigation
        REQUIRE(fs.normalize("./folder\\..\\subfolder/./file.txt") == "subfolder/file.txt");
        REQUIRE(fs.normalize("a\\b/..\\c\\./d/../file.txt") == "a/c/file.txt");

        // Test mixed separators in long paths
        REQUIRE(fs.normalize("root\\level1/level2\\level3/file.txt") == "root/level1/level2/level3/file.txt");
    }

    SECTION("Redundant separators") {
        // Test double forward slashes
        REQUIRE(fs.normalize("folder//file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("folder///file.txt") == "folder/file.txt");

        // Test double backslashes
        REQUIRE(fs.normalize("folder\\\\file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("folder\\\\\\file.txt") == "folder/file.txt");

        // Test mixed redundant separators
        REQUIRE(fs.normalize("folder//\\file.txt") == "folder/file.txt");
        REQUIRE(fs.normalize("folder\\\\//file.txt") == "folder/file.txt");

        // Test redundant separators with navigation
        REQUIRE(fs.normalize("folder//subfolder/..//file.txt") == "folder/file.txt");
    }

    SECTION("Absolute paths") {
        // Test Unix-style absolute paths
        REQUIRE(fs.normalize("/home/user/file.txt") == "/home/user/file.txt");
        REQUIRE(fs.normalize("/home/../usr/file.txt") == "/usr/file.txt");

        // Test root directory navigation
        REQUIRE(fs.normalize("/folder/../file.txt") == "/file.txt");
        REQUIRE(fs.normalize("/folder/subfolder/../../file.txt") == "/file.txt");

        // Test going above root (should stay at root)
        REQUIRE(fs.normalize("/..") == "/");
        REQUIRE(fs.normalize("/../file.txt") == "/file.txt");
        REQUIRE(fs.normalize("/folder/../../file.txt") == "/file.txt");
    }

    SECTION("Edge cases and special inputs") {
        // Test empty string
        REQUIRE(fs.normalize("") == "");

        // Test single characters
        REQUIRE(fs.normalize(".") == ".");
        REQUIRE(fs.normalize("..") == "..");
        REQUIRE(fs.normalize("/") == "/");
        REQUIRE(fs.normalize("\\") == "/");

        // Test only separators and dots
        REQUIRE(fs.normalize("./") == ".");
        REQUIRE(fs.normalize("../") == "..");
        REQUIRE(fs.normalize("./../") == "..");
        REQUIRE(fs.normalize("./..") == "..");

        // Test filename only (no directories)
        REQUIRE(fs.normalize("file.txt") == "file.txt");
        REQUIRE(fs.normalize("complex-file_name.123.txt") == "complex-file_name.123.txt");
    }

    SECTION("Special characters in paths") {
        // Test spaces in filenames and directories
        REQUIRE(fs.normalize("My Documents/My File.txt") == "My Documents/My File.txt");
        REQUIRE(fs.normalize("folder with spaces\\file with spaces.txt") == "folder with spaces/file with spaces.txt");

        // Test special characters (should be preserved)
        REQUIRE(fs.normalize("folder-name_123/file@domain.txt") == "folder-name_123/file@domain.txt");
        REQUIRE(fs.normalize("C++/source files/main.cpp") == "C++/source files/main.cpp");

        // Test Unicode characters (should be preserved)
        REQUIRE(fs.normalize("日本語/ファイル.txt") == "日本語/ファイル.txt");
        REQUIRE(fs.normalize("español/archivo.txt") == "español/archivo.txt");
    }

    SECTION("Long and complex paths") {
        // Test deeply nested path with multiple navigations
        std::string complexPath = "root/level1/level2/level3/../level4/../../level5/./level6/../file.txt";
        REQUIRE(fs.normalize(complexPath) == "root/level1/level5/file.txt");

        // Test path that goes back to root level
        std::string backToRoot = "a/b/c/d/e/f/g/h/../../../../../../../file.txt";
        // Note: When going too far back in relative paths, the implementation
        // keeps the first directory to prevent going above the relative root
        REQUIRE(fs.normalize(backToRoot) == "a/file.txt");

        // Test alternating up and down navigation
        std::string alternating = "a/b/../c/d/../e/f/../g/file.txt";
        REQUIRE(fs.normalize(alternating) == "a/c/e/g/file.txt");
    }

    SECTION("Error handling and malformed paths") {
        // These should not crash and should return reasonable results
        // The implementation handles exceptions and returns the original path

        // Test paths with null characters (if they somehow get through)
        // Note: This might not be testable depending on string handling

        // Test very long paths (should still work)
        std::string veryLongPath;
        for (int i = 0; i < 100; ++i) {
            veryLongPath += "folder" + std::to_string(i) + "/";
        }
        veryLongPath += "file.txt";

        std::string normalizedLong = fs.normalize(veryLongPath);
        REQUIRE(!normalizedLong.empty());
        REQUIRE(normalizedLong.back() != '/'); // Should not end with separator

        // Test path with only dots and separators that might be problematic
        REQUIRE(fs.normalize("./../.././") == "../..");
        REQUIRE(fs.normalize("../../../") == "../../..");
    }

    SECTION("Real-world path examples") {
        // Test typical project structure paths
        REQUIRE(fs.normalize("./src/main.cpp") == "src/main.cpp");
        REQUIRE(fs.normalize("../include/header.h") == "../include/header.h");
        REQUIRE(fs.normalize("./build/../src/main.cpp") == "src/main.cpp");

        // Test resource paths common in game development
        REQUIRE(fs.normalize("./assets/textures/../sounds/music.wav") == "assets/sounds/music.wav");
        REQUIRE(fs.normalize("data\\models\\..\\textures\\diffuse.png") == "data/textures/diffuse.png");

        // Test config file paths
        REQUIRE(fs.normalize("./config/../data/settings.json") == "data/settings.json");
        REQUIRE(fs.normalize("~\\AppData\\..\\Desktop\\file.txt") == "~/Desktop/file.txt");
    }
}

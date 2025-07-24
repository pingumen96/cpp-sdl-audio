#include "IFileSystem.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace resources {

    bool StandardFileSystem::exists(const std::string& path) const {
        return std::filesystem::exists(path);
    }

    size_t StandardFileSystem::size(const std::string& path) const {
        try {
            if (!exists(path)) {
                return 0;
            }
            return std::filesystem::file_size(path);
        } catch (const std::filesystem::filesystem_error&) {
            return 0;
        }
    }

    FileData StandardFileSystem::readAll(const std::string& path) const {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return FileData{};
        }

        // tellg() returns the current position of the get pointer
        auto fileSize = file.tellg();
        if (fileSize <= 0) {
            return FileData{};
        }

        // seekg() moves the get pointer to the beginning of the file
        file.seekg(0, std::ios::beg);

        // read the entire file into a buffer
        std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

        if (!file) {
            return FileData{};
        }

        // the buffer was already created, so move it into FileData
        return FileData{ std::move(buffer) };
    }

    std::string StandardFileSystem::normalize(const std::string& path) const {
        try {
            // First convert all backslashes to forward slashes
            std::string workingPath = path;
            std::replace(workingPath.begin(), workingPath.end(), '\\', '/');

            // Use filesystem path for normalization
            auto normalized = std::filesystem::path(workingPath).lexically_normal();
            std::string result = normalized.string();

            // Convert any remaining backslashes to forward slashes (Windows compatibility)
            std::replace(result.begin(), result.end(), '\\', '/');

            // Remove duplicate slashes
            std::string cleaned;
            cleaned.reserve(result.size());
            bool lastWasSlash = false;

            for (char c : result) {
                if (c == '/') {
                    if (!lastWasSlash) {
                        cleaned += c;
                        lastWasSlash = true;
                    }
                } else {
                    cleaned += c;
                    lastWasSlash = false;
                }
            }

            return cleaned;
        } catch (const std::exception&) {
            return path; // Return original path if normalization fails
        }
    }

} // namespace resources

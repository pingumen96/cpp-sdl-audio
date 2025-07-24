#pragma once

#include <string>
#include <vector>
#include <memory>

namespace resources {

    /**
     * @brief Data container for file content
     */
    struct FileData {
        std::vector<uint8_t> data;

        FileData() = default;
        FileData(std::vector<uint8_t>&& data) : data(std::move(data)) {}

        const uint8_t* getData() const { return data.data(); }
        size_t size() const { return data.size(); }
        bool empty() const { return data.empty(); }
    };

    /**
     * @brief Abstract interface for file system operations
     */
    class IFileSystem {
    public:
        virtual ~IFileSystem() = default;

        /**
         * @brief Check if a file exists
         * @param path File path to check
         * @return true if file exists, false otherwise
         */
        virtual bool exists(const std::string& path) const = 0;

        /**
         * @brief Get file size in bytes
         * @param path File path
         * @return File size or 0 if file doesn't exist
         */
        virtual size_t size(const std::string& path) const = 0;

        /**
         * @brief Read entire file content
         * @param path File path to read
         * @return FileData containing file content, empty if failed
         */
        virtual FileData readAll(const std::string& path) const = 0;

        /**
         * @brief Normalize path (resolve .. and . components, convert separators)
         * @param path Path to normalize
         * @return Normalized path
         */
        virtual std::string normalize(const std::string& path) const = 0;
    };

    /**
     * @brief Standard file system implementation
     */
    class StandardFileSystem : public IFileSystem {
    public:
        bool exists(const std::string& path) const override;
        size_t size(const std::string& path) const override;
        FileData readAll(const std::string& path) const override;
        std::string normalize(const std::string& path) const override;
    };

} // namespace resources

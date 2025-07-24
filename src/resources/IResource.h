#pragma once

#include "IFileSystem.h"
#include <string>
#include <memory>

namespace resources {

    /**
     * @brief Base interface for all loadable resources
     */
    class IResource {
    public:
        virtual ~IResource() = default;

        /**
         * @brief Load resource from file data
         * @param data File data to load from
         * @return true if loading succeeded, false otherwise
         */
        virtual bool onLoad(const FileData& data) = 0;

        /**
         * @brief Unload resource and free associated memory
         */
        virtual void onUnload() = 0;

        /**
         * @brief Check if resource is currently loaded
         * @return true if loaded, false otherwise
         */
        virtual bool isLoaded() const = 0;

        /**
         * @brief Get resource type name for debugging
         * @return Type name as string
         */
        virtual std::string getResourceType() const = 0;
    };

    using ResourcePtr = std::shared_ptr<IResource>;

} // namespace resources

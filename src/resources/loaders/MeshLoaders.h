#pragma once

#include "../IResourceLoader.h"
#include "../types/MeshResource.h"

namespace resources {

    /**
     * @brief Loader for Wavefront OBJ mesh files
     */
    class ObjMeshLoader : public IResourceLoader {
    public:
        bool canLoad(const std::string& path) const override;
        ResourcePtr createEmpty() const override;
        ResourcePtr load(const std::string& path, const IFileSystem& fileSystem) const override;
        std::vector<std::string> getSupportedExtensions() const override;
        std::string getLoaderType() const override;
    };

} // namespace resources

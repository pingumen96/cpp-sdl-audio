#include "MeshLoaders.h"
#include "../LoaderFactory.h"

namespace resources {

    bool ObjMeshLoader::canLoad(const std::string& path) const {
        std::string ext = LoaderFactory::getFileExtension(path);
        return ext == ".obj";
    }

    ResourcePtr ObjMeshLoader::createEmpty() const {
        return std::make_shared<MeshResource>();
    }

    ResourcePtr ObjMeshLoader::load(const std::string& path, const IFileSystem& fileSystem) const {
        if (!canLoad(path) || !fileSystem.exists(path)) {
            return nullptr;
        }

        FileData data = fileSystem.readAll(path);
        if (data.empty()) {
            return nullptr;
        }

        auto mesh = std::make_shared<MeshResource>();
        if (mesh->onLoad(data)) {
            return mesh;
        }

        return nullptr;
    }

    std::vector<std::string> ObjMeshLoader::getSupportedExtensions() const {
        return { ".obj" };
    }

    std::string ObjMeshLoader::getLoaderType() const {
        return "ObjMeshLoader";
    }

} // namespace resources

#pragma once

#include "../IResource.h"
#include <GL/glew.h>
#include <vector>
#include <string>

namespace resources {

    /**
     * @brief Simple vertex structure
     */
    struct Vertex {
        float position[3];  // x, y, z
        float normal[3];    // nx, ny, nz
        float texCoords[2]; // u, v

        Vertex() = default;
        Vertex(float x, float y, float z, float nx = 0.0f, float ny = 0.0f, float nz = 1.0f, float u = 0.0f, float v = 0.0f)
            : position{ x, y, z }, normal{ nx, ny, nz }, texCoords{ u, v } {}
    };

    /**
     * @brief Mesh resource that implements IResource interface
     */
    class MeshResource : public IResource {
    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        GLuint VAO, VBO, EBO;
        bool loaded;
        size_t triangleCount;

        void setupOpenGLBuffers();
        void cleanup();
        bool parseObjData(const uint8_t* data, size_t size);

    public:
        MeshResource();
        ~MeshResource() override;

        // IResource interface implementation
        bool onLoad(const FileData& data) override;
        void onUnload() override;
        bool isLoaded() const override { return loaded; }
        std::string getResourceType() const override { return "Mesh"; }

        // Mesh-specific methods
        GLuint getVAO() const { return VAO; }
        GLuint getVBO() const { return VBO; }
        GLuint getEBO() const { return EBO; }

        size_t getVertexCount() const { return vertices.size(); }
        size_t getIndexCount() const { return indices.size(); }
        size_t getTriangleCount() const { return triangleCount; }

        const std::vector<Vertex>& getVertices() const { return vertices; }
        const std::vector<unsigned int>& getIndices() const { return indices; }

        void render() const;

        // Prevent copy
        MeshResource(const MeshResource&) = delete;
        MeshResource& operator=(const MeshResource&) = delete;

        // Allow move
        MeshResource(MeshResource&& other) noexcept;
        MeshResource& operator=(MeshResource&& other) noexcept;
    };

} // namespace resources

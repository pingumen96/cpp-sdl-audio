#include "MeshResource.h"
#include <sstream>
#include <iostream>

namespace resources {

    MeshResource::MeshResource()
        : VAO(0), VBO(0), EBO(0), loaded(false), triangleCount(0) {}

    MeshResource::~MeshResource() {
        cleanup();
    }

    bool MeshResource::onLoad(const FileData& data) {
        if (data.empty()) {
            return false;
        }

        try {
            if (!parseObjData(data.getData(), data.size())) {
                return false;
            }

            setupOpenGLBuffers();
            loaded = true;
            triangleCount = indices.size() / 3;
            return true;
        } catch (const std::exception&) {
            cleanup();
            loaded = false;
            return false;
        }
    }

    void MeshResource::onUnload() {
        cleanup();
        loaded = false;
    }

    bool MeshResource::parseObjData(const uint8_t* data, size_t size) {
        // Convert data to string for parsing
        std::string objContent(reinterpret_cast<const char*>(data), size);
        std::istringstream stream(objContent);
        std::string line;

        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> texCoords;

        vertices.clear();
        indices.clear();

        while (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            std::string type;
            lineStream >> type;

            if (type == "v") {
                // Vertex position
                float x, y, z;
                lineStream >> x >> y >> z;
                positions.push_back(x);
                positions.push_back(y);
                positions.push_back(z);
            } else if (type == "vn") {
                // Vertex normal
                float x, y, z;
                lineStream >> x >> y >> z;
                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(z);
            } else if (type == "vt") {
                // Texture coordinate
                float u, v;
                lineStream >> u >> v;
                texCoords.push_back(u);
                texCoords.push_back(v);
            } else if (type == "f") {
                // Face (simplified - assumes triangular faces)
                std::string vertex1, vertex2, vertex3;
                lineStream >> vertex1 >> vertex2 >> vertex3;

                // Parse vertex indices (format: v/vt/vn)
                auto parseVertex = [&](const std::string& vertexStr) {
                    size_t pos1 = vertexStr.find('/');
                    size_t pos2 = vertexStr.find('/', pos1 + 1);

                    int vIdx = std::stoi(vertexStr.substr(0, pos1)) - 1; // OBJ is 1-indexed
                    int vtIdx = -1;
                    int vnIdx = -1;

                    if (pos1 != std::string::npos && pos2 != std::string::npos) {
                        if (pos2 > pos1 + 1) {
                            vtIdx = std::stoi(vertexStr.substr(pos1 + 1, pos2 - pos1 - 1)) - 1;
                        }
                        vnIdx = std::stoi(vertexStr.substr(pos2 + 1)) - 1;
                    }

                    Vertex vertex;

                    // Position
                    if (vIdx >= 0 && vIdx * 3 + 2 < positions.size()) {
                        vertex.position[0] = positions[vIdx * 3];
                        vertex.position[1] = positions[vIdx * 3 + 1];
                        vertex.position[2] = positions[vIdx * 3 + 2];
                    }

                    // Normal
                    if (vnIdx >= 0 && vnIdx * 3 + 2 < normals.size()) {
                        vertex.normal[0] = normals[vnIdx * 3];
                        vertex.normal[1] = normals[vnIdx * 3 + 1];
                        vertex.normal[2] = normals[vnIdx * 3 + 2];
                    }

                    // Texture coordinates
                    if (vtIdx >= 0 && vtIdx * 2 + 1 < texCoords.size()) {
                        vertex.texCoords[0] = texCoords[vtIdx * 2];
                        vertex.texCoords[1] = texCoords[vtIdx * 2 + 1];
                    }

                    return vertex;
                    };

                // Add vertices and indices
                size_t baseIndex = vertices.size();
                vertices.push_back(parseVertex(vertex1));
                vertices.push_back(parseVertex(vertex2));
                vertices.push_back(parseVertex(vertex3));

                indices.push_back(static_cast<unsigned int>(baseIndex));
                indices.push_back(static_cast<unsigned int>(baseIndex + 1));
                indices.push_back(static_cast<unsigned int>(baseIndex + 2));
            }
        }

        return !vertices.empty();
    }

    void MeshResource::setupOpenGLBuffers() {
        // Generate and bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and setup VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Generate and setup EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Setup vertex attributes
        // Position attribute (location 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        // Normal attribute (location 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute (location 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);

        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cleanup();
            throw std::runtime_error("OpenGL error during mesh setup: " + std::to_string(error));
        }
    }

    void MeshResource::cleanup() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO != 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }

        vertices.clear();
        indices.clear();
        triangleCount = 0;
    }

    void MeshResource::render() const {
        if (loaded && VAO != 0) {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    MeshResource::MeshResource(MeshResource&& other) noexcept
        : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
        VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), loaded(other.loaded),
        triangleCount(other.triangleCount) {
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.loaded = false;
        other.triangleCount = 0;
    }

    MeshResource& MeshResource::operator=(MeshResource&& other) noexcept {
        if (this != &other) {
            cleanup();

            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            loaded = other.loaded;
            triangleCount = other.triangleCount;

            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
            other.loaded = false;
            other.triangleCount = 0;
        }
        return *this;
    }

} // namespace resources

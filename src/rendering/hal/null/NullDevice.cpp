#include "NullDevice.h"
#include <algorithm>
#include <sstream>

namespace rendering::hal {

    bool NullDevice::initialize(uint32_t width, uint32_t height) {
        if (initialized) {
            return true;
        }

        recordOperation("initialize(" + std::to_string(width) + ", " + std::to_string(height) + ")");
        viewportWidth = static_cast<int>(width);
        viewportHeight = static_cast<int>(height);
        initialized = true;
        return true;
    }

    void NullDevice::shutdown() {
        if (!initialized) {
            return;
        }

        recordOperation("shutdown()");
        initialized = false;
        textures.clear();
        shaderPrograms.clear();
    }

    void NullDevice::beginFrame() {
        recordOperation("beginFrame()");
    }

    void NullDevice::endFrame() {
        recordOperation("endFrame()");
    }

    void NullDevice::clear(float r, float g, float b, float a) {
        std::stringstream ss;
        ss << "clear(" << r << ", " << g << ", " << b << ", " << a << ")";
        recordOperation(ss.str());
    }

    void NullDevice::setViewport(int x, int y, int width, int height) {
        recordOperation("setViewport(" + std::to_string(x) + ", " + std::to_string(y) +
            ", " + std::to_string(width) + ", " + std::to_string(height) + ")");
        viewportWidth = width;
        viewportHeight = height;
    }

    void NullDevice::getViewportSize(int& width, int& height) const {
        width = viewportWidth;
        height = viewportHeight;
        recordOperation("getViewportSize()");
    }

    void NullDevice::setProjectionMatrix(const math::Mat4& projection) {
        projectionMatrix = projection;
        recordOperation("setProjectionMatrix()");
    }

    void NullDevice::setViewMatrix(const math::Mat4& view) {
        viewMatrix = view;
        recordOperation("setViewMatrix()");
    }

    void NullDevice::drawQuad(const math::Mat4& modelMatrix,
        float r, float g, float b, float a) {
        std::stringstream ss;
        ss << "drawQuad(color=" << r << "," << g << "," << b << "," << a << ")";
        recordOperation(ss.str());
    }

    void NullDevice::drawTexturedQuad(const math::Mat4& modelMatrix,
        uint32_t textureId) {
        recordOperation("drawTexturedQuad(texture=" + std::to_string(textureId) + ")");
    }

    uint32_t NullDevice::createTexture(const unsigned char* data,
        int width, int height, int channels) {
        uint32_t id = nextTextureId++;
        std::stringstream ss;
        ss << "texture_" << width << "x" << height << "_" << channels << "ch";
        textures[id] = ss.str();

        recordOperation("createTexture(id=" + std::to_string(id) + ", " + ss.str() + ")");
        return id;
    }

    void NullDevice::deleteTexture(uint32_t textureId) {
        recordOperation("deleteTexture(id=" + std::to_string(textureId) + ")");
        textures.erase(textureId);
    }

    uint32_t NullDevice::createShaderProgram(const std::string& vertexSource,
        const std::string& fragmentSource) {
        uint32_t id = nextShaderId++;
        shaderPrograms[id] = "vertex:" + std::to_string(vertexSource.length()) +
            ",fragment:" + std::to_string(fragmentSource.length());

        recordOperation("createShaderProgram(id=" + std::to_string(id) + ")");
        return id;
    }

    void NullDevice::deleteShaderProgram(uint32_t programId) {
        recordOperation("deleteShaderProgram(id=" + std::to_string(programId) + ")");
        shaderPrograms.erase(programId);
    }

    std::string NullDevice::getDeviceName() const {
        recordOperation("getDeviceName()");
        return "Null Render Device (Testing)";
    }

    bool NullDevice::supportsFeature(const std::string& feature) const {
        recordOperation("supportsFeature(" + feature + ")");
        // Null device "supports" everything for testing
        return true;
    }

    bool NullDevice::hasOperation(const std::string& operation) const {
        return std::find(operations.begin(), operations.end(), operation) != operations.end();
    }

    size_t NullDevice::countOperations(const std::string& prefix) const {
        return std::count_if(operations.begin(), operations.end(),
            [&prefix](const std::string& op) {
                return op.substr(0, prefix.length()) == prefix;
            });
    }

    void NullDevice::recordOperation(const std::string& operation) const {
        operations.push_back(operation);
    }

} // namespace rendering::hal

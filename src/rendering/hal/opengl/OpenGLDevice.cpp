#include "OpenGLDevice.h"
#include "../../../core/Window.h"
#include <iostream>
#include <sstream>

namespace rendering::hal {

    // Default shaders
    static const char* VERTEX_SHADER_COLOR = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
)";

    static const char* FRAGMENT_SHADER_COLOR = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 u_Color;

void main() {
    FragColor = u_Color;
}
)";

    static const char* VERTEX_SHADER_TEXTURE = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_MVP;

out vec2 TexCoord;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

    static const char* FRAGMENT_SHADER_TEXTURE = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D u_Texture;

void main() {
    FragColor = texture(u_Texture, TexCoord);
}
)";

    OpenGLDevice::OpenGLDevice(core::Window& window)
        : window(&window) {}

    OpenGLDevice::~OpenGLDevice() {
        if (initialized) {
            shutdown();
        }
    }

    bool OpenGLDevice::initialize(uint32_t width, uint32_t height) {
        if (initialized) {
            return true;
        }

        // Initialize GLEW (assuming it's done elsewhere for now)
        // This would typically be done in Window class

        std::cout << "[OpenGLDevice] Initializing OpenGL device..." << std::endl;
        std::cout << "[OpenGLDevice] OpenGL Version: " << getOpenGLVersion() << std::endl;
        std::cout << "[OpenGLDevice] Device: " << getDeviceName() << std::endl;

        // Set initial viewport
        setViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

        // Initialize rendering resources
        initializeQuadResources();

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set default matrices
        projectionMatrix = math::Mat4::identity();
        viewMatrix = math::Mat4::identity();

        initialized = true;
        return true;
    }

    void OpenGLDevice::shutdown() {
        if (!initialized) {
            return;
        }

        std::cout << "[OpenGLDevice] Shutting down OpenGL device..." << std::endl;

        cleanupQuadResources();
        initialized = false;
    }

    void OpenGLDevice::beginFrame() {
        // Nothing specific needed for OpenGL frame begin
    }

    void OpenGLDevice::endFrame() {
        // Swap buffers is handled by the Window
        window->swapBuffers();
    }

    void OpenGLDevice::clear(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLDevice::setViewport(int x, int y, int width, int height) {
        viewportX = x;
        viewportY = y;
        viewportWidth = width;
        viewportHeight = height;
        glViewport(x, y, width, height);
    }

    void OpenGLDevice::getViewportSize(int& width, int& height) const {
        width = viewportWidth;
        height = viewportHeight;
    }

    void OpenGLDevice::setProjectionMatrix(const math::Mat4& projection) {
        projectionMatrix = projection;
    }

    void OpenGLDevice::setViewMatrix(const math::Mat4& view) {
        viewMatrix = view;
    }

    void OpenGLDevice::drawQuad(const math::Mat4& modelMatrix,
        float r, float g, float b, float a) {
        if (!initialized) return;

        glUseProgram(simpleColorProgram);

        // Set MVP matrix
        math::Mat4 mvp = calculateMVP(modelMatrix);
        glUniformMatrix4fv(colorProgram_mvpLocation, 1, GL_FALSE, mvp.data());

        // Set color
        glUniform4f(colorProgram_colorLocation, r, g, b, a);

        // Draw quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void OpenGLDevice::drawTexturedQuad(const math::Mat4& modelMatrix,
        uint32_t textureId) {
        if (!initialized) return;

        glUseProgram(simpleTextureProgram);

        // Set MVP matrix
        math::Mat4 mvp = calculateMVP(modelMatrix);
        glUniformMatrix4fv(textureProgram_mvpLocation, 1, GL_FALSE, mvp.data());

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUniform1i(textureProgram_textureLocation, 0);

        // Draw quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    uint32_t OpenGLDevice::createTexture(const unsigned char* data,
        int width, int height, int channels) {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        return static_cast<uint32_t>(textureId);
    }

    void OpenGLDevice::deleteTexture(uint32_t textureId) {
        GLuint id = static_cast<GLuint>(textureId);
        glDeleteTextures(1, &id);
    }

    uint32_t OpenGLDevice::createShaderProgram(const std::string& vertexSource,
        const std::string& fragmentSource) {
        GLuint program = glCreateProgram();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Compile shaders
        if (!compileShader(vertexShader, vertexSource)) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
            return 0;
        }

        if (!compileShader(fragmentShader, fragmentSource)) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
            return 0;
        }

        // Link program
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        if (!linkProgram(program)) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
            return 0;
        }

        // Cleanup shaders (they're linked into the program now)
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return static_cast<uint32_t>(program);
    }

    void OpenGLDevice::deleteShaderProgram(uint32_t programId) {
        GLuint id = static_cast<GLuint>(programId);
        glDeleteProgram(id);
    }

    std::string OpenGLDevice::getDeviceName() const {
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* vendor = glGetString(GL_VENDOR);

        std::stringstream ss;
        ss << vendor << " " << renderer;
        return ss.str();
    }

    bool OpenGLDevice::supportsFeature(const std::string& feature) const {
        // Simple feature detection - can be expanded
        if (feature == "textures") return true;
        if (feature == "blending") return true;
        if (feature == "shaders") return true;
        return false;
    }

    std::string OpenGLDevice::getOpenGLVersion() const {
        const GLubyte* version = glGetString(GL_VERSION);
        return std::string(reinterpret_cast<const char*>(version));
    }

    // Private methods

    void OpenGLDevice::initializeQuadResources() {
        // Quad vertices (position + texture coordinates)
        float quadVertices[] = {
            // positions   // texture coords
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // bottom left
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // bottom right
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // top left
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f   // top right
        };

        // Generate and bind VAO
        glGenVertexArrays(1, &quadVAO);
        glBindVertexArray(quadVAO);

        // Generate and bind VBO
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // Create shader programs
        simpleColorProgram = createShaderProgram(VERTEX_SHADER_COLOR, FRAGMENT_SHADER_COLOR);
        simpleTextureProgram = createShaderProgram(VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE);

        // Get uniform locations
        colorProgram_mvpLocation = glGetUniformLocation(simpleColorProgram, "u_MVP");
        colorProgram_colorLocation = glGetUniformLocation(simpleColorProgram, "u_Color");
        textureProgram_mvpLocation = glGetUniformLocation(simpleTextureProgram, "u_MVP");
        textureProgram_textureLocation = glGetUniformLocation(simpleTextureProgram, "u_Texture");
    }

    void OpenGLDevice::cleanupQuadResources() {
        if (quadVAO != 0) {
            glDeleteVertexArrays(1, &quadVAO);
            quadVAO = 0;
        }

        if (quadVBO != 0) {
            glDeleteBuffers(1, &quadVBO);
            quadVBO = 0;
        }

        if (simpleColorProgram != 0) {
            glDeleteProgram(simpleColorProgram);
            simpleColorProgram = 0;
        }

        if (simpleTextureProgram != 0) {
            glDeleteProgram(simpleTextureProgram);
            simpleTextureProgram = 0;
        }
    }

    bool OpenGLDevice::compileShader(GLuint shader, const std::string& source) {
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "[OpenGLDevice] Shader compilation error: " << infoLog << std::endl;
            return false;
        }

        return true;
    }

    bool OpenGLDevice::linkProgram(GLuint program) {
        glLinkProgram(program);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "[OpenGLDevice] Program linking error: " << infoLog << std::endl;
            return false;
        }

        return true;
    }

    math::Mat4 OpenGLDevice::calculateMVP(const math::Mat4& modelMatrix) const {
        return projectionMatrix * viewMatrix * modelMatrix;
    }

} // namespace rendering::hal

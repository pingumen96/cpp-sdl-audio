#include "OpenGLBackend.h"
#include "Window.h"
#include "../scene/rendering/CommandBuffer.h"
#include <GL/glew.h>
#include <iostream>

namespace core {

    OpenGLBackend::OpenGLBackend(Window& win) : window(&win) {
        // Initialize matrices
        projectionMatrix = math::Mat4::identity();
        viewMatrix = math::Mat4::identity();
    }

    OpenGLBackend::~OpenGLBackend() {
        shutdown();
    }

    bool OpenGLBackend::init(uint32_t w, uint32_t h) {
        this->width = w;
        this->height = h;

        std::cout << "[OpenGLBackend] Initializing with dimensions: " << w << "x" << h << std::endl;

        // Set initial viewport
        setViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

        // Set up default projection matrix (2D orthographic) - centered like Camera2D
        float halfWidth = static_cast<float>(width) * 0.5f;
        float halfHeight = static_cast<float>(height) * 0.5f;
        projectionMatrix = math::Mat4::ortho(-halfWidth, halfWidth,
            -halfHeight, halfHeight,
            -1.0f, 1.0f);

        std::cout << "[OpenGLBackend] Projection matrix (ortho -" << halfWidth << " to " << halfWidth << ", -" << halfHeight << " to " << halfHeight << "):" << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << "  [" << projectionMatrix(i, 0) << ", " << projectionMatrix(i, 1)
                << ", " << projectionMatrix(i, 2) << ", " << projectionMatrix(i, 3) << "]" << std::endl;
        }

        // Set up identity view matrix
        viewMatrix = math::Mat4::identity();

        // Set default clear color to dark gray to see objects better
        setDrawColor(0.1f, 0.1f, 0.1f, 1.0f);
        std::cout << "[OpenGLBackend] Clear color set to dark gray (0.1, 0.1, 0.1, 1.0)" << std::endl;

        // Initialize quad rendering resources
        initializeQuadRendering();

        initialized = true;
        std::cout << "[OpenGLBackend] Initialization complete" << std::endl;
        return true;
    }

    bool OpenGLBackend::beginFrame() {
        if (!initialized) {
            return false;
        }

        clear();
        return true;
    }

    bool OpenGLBackend::submit(const scene::CommandBuffer& commandBuffer) {
        if (!initialized) {
            return false;
        }

        // Get resource manager from scene manager (we need to add this access)
        // For now, let's create a temporary workaround

        // Get camera and apply matrices
        const auto& camera = commandBuffer.getCamera();
        setProjectionMatrix(camera.projectionMatrix);
        setViewMatrix(camera.viewMatrix);

        std::cout << "[OpenGLBackend] Camera matrices received:" << std::endl;
        std::cout << "  Projection:" << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << "    [" << camera.projectionMatrix(i, 0) << ", " << camera.projectionMatrix(i, 1)
                << ", " << camera.projectionMatrix(i, 2) << ", " << camera.projectionMatrix(i, 3) << "]" << std::endl;
        }
        std::cout << "  View:" << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << "    [" << camera.viewMatrix(i, 0) << ", " << camera.viewMatrix(i, 1)
                << ", " << camera.viewMatrix(i, 2) << ", " << camera.viewMatrix(i, 3) << "]" << std::endl;
        }

        // Process draw items - Actual rendering implementation
        const auto& drawItems = commandBuffer.getDrawItems();
        if (!drawItems.empty()) {
            std::cout << "[OpenGLBackend] Rendering " << drawItems.size() << " draw items" << std::endl;

            for (const auto& item : drawItems) {
                // For now, we'll implement basic quad rendering
                // The mesh should be "unit_quad_2d" for all 2D objects
                if (item.meshId == "unit_quad_2d") {
                    renderUnitQuad(item);
                } else {
                    std::cout << "[OpenGLBackend] Warning: Unknown mesh '" << item.meshId << "', skipping" << std::endl;
                }
            }
        }

        // Process UI items (basic logging for now)
        const auto& uiItems = commandBuffer.getUIItems();
        if (!uiItems.empty()) {
            std::cout << "[OpenGLBackend] Processing " << uiItems.size() << " UI items" << std::endl;
            // TODO: Implement actual UI rendering
        }

        // Process post effects (basic logging for now)
        const auto& postEffects = commandBuffer.getPostEffects();
        if (!postEffects.empty()) {
            std::cout << "[OpenGLBackend] Processing " << postEffects.size() << " post effects" << std::endl;
            // TODO: Implement actual post-processing
        }

        return true;
    }

    bool OpenGLBackend::present() {
        if (!initialized || !window) {
            return false;
        }

        window->swapBuffers();
        return true;
    }

    void OpenGLBackend::shutdown() {
        if (initialized) {
            cleanupQuadRendering();
            initialized = false;
        }
    }

    bool OpenGLBackend::resize(uint32_t newWidth, uint32_t newHeight) {
        if (!initialized) {
            return false;
        }

        width = newWidth;
        height = newHeight;

        setViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

        // Update projection matrix to match Camera2D style (centered)
        float halfWidth = static_cast<float>(width) * 0.5f;
        float halfHeight = static_cast<float>(height) * 0.5f;
        projectionMatrix = math::Mat4::ortho(-halfWidth, halfWidth,
            -halfHeight, halfHeight,
            -1.0f, 1.0f);

        return true;
    }

    std::string OpenGLBackend::getBackendType() const {
        return "OpenGL";
    }

    bool OpenGLBackend::isInitialized() const {
        return initialized;
    }

    // Additional methods for compatibility
    void OpenGLBackend::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLBackend::setDrawColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void OpenGLBackend::setViewport(int x, int y, int w, int h) {
        glViewport(x, y, w, h);
    }

    void OpenGLBackend::getViewportSize(int& w, int& h) const {
        w = static_cast<int>(width);
        h = static_cast<int>(height);
    }

    void OpenGLBackend::setProjectionMatrix(const math::Mat4& projection) {
        projectionMatrix = projection;
    }

    void OpenGLBackend::setViewMatrix(const math::Mat4& view) {
        viewMatrix = view;
    }

    void OpenGLBackend::initializeQuadRendering() {
        // Create a simple unit quad geometry
        float quadVertices[] = {
            // positions   // texture coords
            -0.5f, -0.5f,  0.0f, 0.0f,  // bottom left
             0.5f, -0.5f,  1.0f, 0.0f,  // bottom right
             0.5f,  0.5f,  1.0f, 1.0f,  // top right

             0.5f,  0.5f,  1.0f, 1.0f,  // top right
            -0.5f,  0.5f,  0.0f, 1.0f,  // top left
            -0.5f, -0.5f,  0.0f, 0.0f   // bottom left
        };

        // Generate VAO and VBO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // Create a simple shader program
        const char* vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "layout (location = 1) in vec2 aTexCoord;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "out vec2 TexCoord;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);\n"
            "   TexCoord = aTexCoord;\n"
            "}\0";

        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoord;\n"
            "uniform vec4 color;\n"
            "void main()\n"
            "{\n"
            "   FragColor = color;\n"
            "}\0";

        // Compile vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        // Compile fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        // Create shader program
        simpleShaderProgram = glCreateProgram();
        glAttachShader(simpleShaderProgram, vertexShader);
        glAttachShader(simpleShaderProgram, fragmentShader);
        glLinkProgram(simpleShaderProgram);

        // Clean up shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void OpenGLBackend::renderUnitQuad(const scene::DrawItem& item) {
        // Use our simple shader
        glUseProgram(simpleShaderProgram);

        // Extract position and scale from model matrix for debugging
        float posX = item.modelMatrix(0, 3);
        float posY = item.modelMatrix(1, 3);
        float scaleX = item.modelMatrix(0, 0);
        float scaleY = item.modelMatrix(1, 1);

        std::cout << "[OpenGLBackend] Rendering quad at (" << posX << ", " << posY << ") scale(" << scaleX << ", " << scaleY << ") layer=" << item.renderLayer << std::endl;

        // Set matrices
        glUniformMatrix4fv(glGetUniformLocation(simpleShaderProgram, "model"), 1, GL_FALSE, item.modelMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(simpleShaderProgram, "view"), 1, GL_FALSE, viewMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(simpleShaderProgram, "projection"), 1, GL_FALSE, projectionMatrix.data());

        // Use different colors based on layer and position
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

        if (item.renderLayer == 1) {
            // Center line - white
            r = 1.0f; g = 1.0f; b = 1.0f;
            std::cout << "  -> Center line (white)" << std::endl;
        } else if (item.renderLayer == 2) {
            // Paddles - distinguish by position
            if (posX < 400.0f) {
                // Left paddle - red
                r = 1.0f; g = 0.2f; b = 0.2f;
                std::cout << "  -> Left paddle (red)" << std::endl;
            } else {
                // Right paddle - blue
                r = 0.2f; g = 0.4f; b = 1.0f;
                std::cout << "  -> Right paddle (blue)" << std::endl;
            }
        } else if (item.renderLayer == 3) {
            // Ball - yellow/green
            r = 0.2f; g = 1.0f; b = 0.2f;
            std::cout << "  -> Ball (green)" << std::endl;
        } else {
            // Fallback - cyan
            r = 0.0f; g = 1.0f; b = 1.0f;
            std::cout << "  -> Unknown layer " << item.renderLayer << " (cyan)" << std::endl;
        }

        glUniform4f(glGetUniformLocation(simpleShaderProgram, "color"), r, g, b, a);

        // Check for OpenGL errors before drawing
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "[OpenGLBackend] OpenGL error before draw: " << error << std::endl;
        }

        // Draw the quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Check for OpenGL errors after drawing
        error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "[OpenGLBackend] OpenGL error after draw: " << error << std::endl;
        }
    }

    void OpenGLBackend::cleanupQuadRendering() {
        if (quadVAO != 0) {
            glDeleteVertexArrays(1, &quadVAO);
            quadVAO = 0;
        }
        if (quadVBO != 0) {
            glDeleteBuffers(1, &quadVBO);
            quadVBO = 0;
        }
        if (simpleShaderProgram != 0) {
            glDeleteProgram(simpleShaderProgram);
            simpleShaderProgram = 0;
        }
    }

}

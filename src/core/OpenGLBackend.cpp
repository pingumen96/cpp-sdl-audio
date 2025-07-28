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
        // Use a wider Z range to accommodate camera positioning
        float halfWidth = static_cast<float>(width) * 0.5f;
        float halfHeight = static_cast<float>(height) * 0.5f;
        projectionMatrix = math::Mat4::ortho(-halfWidth, halfWidth,
            -halfHeight, halfHeight,
            -100.0f, 100.0f);  // Much wider Z range for 2D

        std::cout << "[OpenGLBackend] Projection matrix (ortho -" << halfWidth << " to " << halfWidth << ", -" << halfHeight << " to " << halfHeight << ", Z: -100 to +100):" << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << "  [" << projectionMatrix(i, 0) << ", " << projectionMatrix(i, 1)
                << ", " << projectionMatrix(i, 2) << ", " << projectionMatrix(i, 3) << "]" << std::endl;
        }

        // Set up identity view matrix
        viewMatrix = math::Mat4::identity();

        // Set default clear color to dark gray to see objects better
        setDrawColor(0.1f, 0.1f, 0.1f, 1.0f);
        std::cout << "[OpenGLBackend] Clear color set to dark gray (0.1, 0.1, 0.1, 1.0)" << std::endl;

        // Enable depth testing for proper 2D layering
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        std::cout << "[OpenGLBackend] Depth testing enabled" << std::endl;

        // Disable backface culling for 2D quads
        glDisable(GL_CULL_FACE);
        std::cout << "[OpenGLBackend] Backface culling disabled" << std::endl;

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
        // Use a wider Z range to accommodate camera positioning
        float halfWidth = static_cast<float>(width) * 0.5f;
        float halfHeight = static_cast<float>(height) * 0.5f;
        projectionMatrix = math::Mat4::ortho(-halfWidth, halfWidth,
            -halfHeight, halfHeight,
            -100.0f, 100.0f);  // Much wider Z range for 2D

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

        GLint ok;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetShaderInfoLog(vertexShader, 1024, nullptr, log);
            std::cout << "[SHADER] Vertex compile error:\n" << log << std::endl;

            // crash
            throw std::runtime_error("Vertex shader compilation failed");
        }

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

        // Check for shader program validity
        GLint valid;
        glValidateProgram(simpleShaderProgram);
        glGetProgramiv(simpleShaderProgram, GL_VALIDATE_STATUS, &valid);
        if (!valid) {
            std::cout << "[OpenGLBackend] WARNING: Shader program validation failed!" << std::endl;
        }

        // Extract position and scale from model matrix for debugging
        float posX = item.modelMatrix(0, 3);
        float posY = item.modelMatrix(1, 3);
        float scaleX = item.modelMatrix(0, 0);
        float scaleY = item.modelMatrix(1, 1);

        // Check if quad is in viewport bounds
        bool inViewport = isQuadInViewport(posX, posY, scaleX, scaleY);

        std::cout << "[OpenGLBackend] Rendering quad at (" << posX << ", " << posY << ") scale(" << scaleX << ", " << scaleY << ") layer=" << item.renderLayer;
        if (!inViewport) {
            std::cout << " [WARNING: OUTSIDE VIEWPORT!]";
        }
        std::cout << std::endl;

        // Set matrices
        GLint modelLoc = glGetUniformLocation(simpleShaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(simpleShaderProgram, "view");
        GLint projLoc = glGetUniformLocation(simpleShaderProgram, "projection");
        GLint colorLoc = glGetUniformLocation(simpleShaderProgram, "color");

        if (modelLoc == -1 || viewLoc == -1 || projLoc == -1 || colorLoc == -1) {
            std::cout << "[OpenGLBackend] WARNING: Some uniform locations not found! model=" << modelLoc
                << " view=" << viewLoc << " proj=" << projLoc << " color=" << colorLoc << std::endl;
        }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, item.modelMatrix.data());
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix.data());
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projectionMatrix.data());

        // Use different colors based on layer
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

        switch (item.renderLayer) {
        case 0:
            r = 0.8f; g = 0.8f; b = 0.8f; // Light gray
            std::cout << "  -> Layer 0 (light gray)" << std::endl;
            break;
        case 1:
            r = 1.0f; g = 1.0f; b = 1.0f; // White
            std::cout << "  -> Layer 1 (white)" << std::endl;
            break;
        case 2:
            r = 0.2f; g = 0.4f; b = 1.0f; // Blue
            std::cout << "  -> Layer 2 (blue)" << std::endl;
            break;
        case 3:
            r = 0.2f; g = 1.0f; b = 0.2f; // Green
            std::cout << "  -> Layer 3 (green)" << std::endl;
            break;
        case 4:
            r = 1.0f; g = 0.2f; b = 0.2f; // Red
            std::cout << "  -> Layer 4 (red)" << std::endl;
            break;
        case 5:
            r = 1.0f; g = 1.0f; b = 0.2f; // Yellow
            std::cout << "  -> Layer 5 (yellow)" << std::endl;
            break;
        default:
            r = 0.0f; g = 1.0f; b = 1.0f; // Cyan fallback
            std::cout << "  -> Layer " << item.renderLayer << " (cyan)" << std::endl;
            break;
        }

        glUniform4f(colorLoc, r, g, b, a);

        // Check for OpenGL errors before drawing
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "[OpenGLBackend] OpenGL error before draw: " << error << std::endl;
        }

        // Enable wireframe mode temporarily to see if geometry is being drawn
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Draw the quad
        glBindVertexArray(quadVAO);

        // Verify VAO is valid
        GLint currentVAO;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
        if (currentVAO != quadVAO) {
            std::cout << "[OpenGLBackend] WARNING: VAO binding failed! Expected " << quadVAO << " got " << currentVAO << std::endl;
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Reset to fill mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

    bool OpenGLBackend::isQuadInViewport(float posX, float posY, float scaleX, float scaleY) const {
        // Calculate the bounds of the viewport (orthographic projection)
        float halfWidth = static_cast<float>(width) * 0.5f;
        float halfHeight = static_cast<float>(height) * 0.5f;

        float viewportLeft = -halfWidth;
        float viewportRight = halfWidth;
        float viewportBottom = -halfHeight;
        float viewportTop = halfHeight;

        // Calculate the bounds of the quad (unit quad scaled and positioned)
        float quadHalfWidth = scaleX * 0.5f;
        float quadHalfHeight = scaleY * 0.5f;

        float quadLeft = posX - quadHalfWidth;
        float quadRight = posX + quadHalfWidth;
        float quadBottom = posY - quadHalfHeight;
        float quadTop = posY + quadHalfHeight;

        // Check if quad overlaps with viewport
        bool overlapsX = (quadRight >= viewportLeft && quadLeft <= viewportRight);
        bool overlapsY = (quadTop >= viewportBottom && quadBottom <= viewportTop);

        bool inViewport = overlapsX && overlapsY;

        // Debug output
        std::cout << "    [Viewport Debug] Quad bounds: [" << quadLeft << ", " << quadBottom << "] to [" << quadRight << ", " << quadTop << "]" << std::endl;
        std::cout << "    [Viewport Debug] Viewport bounds: [" << viewportLeft << ", " << viewportBottom << "] to [" << viewportRight << ", " << viewportTop << "]" << std::endl;
        std::cout << "    [Viewport Debug] Overlaps X: " << (overlapsX ? "YES" : "NO") << ", Overlaps Y: " << (overlapsY ? "YES" : "NO") << std::endl;

        return inViewport;
    }

}

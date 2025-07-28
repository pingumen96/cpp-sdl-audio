#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "../../math/Matrix.h"

namespace rendering::backend {

    /**
     * @brief Types of draw commands
     */
    enum class DrawCommandType {
        CLEAR,
        DRAW_QUAD,
        DRAW_TEXTURED_QUAD,
        SET_VIEWPORT,
        SET_PROJECTION_MATRIX,
        SET_VIEW_MATRIX
    };

    /**
     * @brief Base draw command
     */
    struct DrawCommand {
        DrawCommandType type;

        DrawCommand(DrawCommandType t) : type(t) {}
        virtual ~DrawCommand() = default;
    };

    /**
     * @brief Clear command
     */
    struct ClearCommand : public DrawCommand {
        float r, g, b, a;

        ClearCommand(float red, float green, float blue, float alpha)
            : DrawCommand(DrawCommandType::CLEAR), r(red), g(green), b(blue), a(alpha) {}
    };

    /**
     * @brief Colored quad draw command
     */
    struct DrawQuadCommand : public DrawCommand {
        math::Mat4 modelMatrix;
        float r, g, b, a;

        DrawQuadCommand(const math::Mat4& model, float red, float green, float blue, float alpha)
            : DrawCommand(DrawCommandType::DRAW_QUAD), modelMatrix(model), r(red), g(green), b(blue), a(alpha) {}
    };

    /**
     * @brief Textured quad draw command
     */
    struct DrawTexturedQuadCommand : public DrawCommand {
        math::Mat4 modelMatrix;
        uint32_t textureId;

        DrawTexturedQuadCommand(const math::Mat4& model, uint32_t texture)
            : DrawCommand(DrawCommandType::DRAW_TEXTURED_QUAD), modelMatrix(model), textureId(texture) {}
    };

    /**
     * @brief Viewport command
     */
    struct ViewportCommand : public DrawCommand {
        int x, y, width, height;

        ViewportCommand(int x_, int y_, int width_, int height_)
            : DrawCommand(DrawCommandType::SET_VIEWPORT), x(x_), y(y_), width(width_), height(height_) {}
    };

    /**
     * @brief Projection matrix command
     */
    struct ProjectionMatrixCommand : public DrawCommand {
        math::Mat4 matrix;

        ProjectionMatrixCommand(const math::Mat4& mat)
            : DrawCommand(DrawCommandType::SET_PROJECTION_MATRIX), matrix(mat) {}
    };

    /**
     * @brief View matrix command
     */
    struct ViewMatrixCommand : public DrawCommand {
        math::Mat4 matrix;

        ViewMatrixCommand(const math::Mat4& mat)
            : DrawCommand(DrawCommandType::SET_VIEW_MATRIX), matrix(mat) {}
    };

    /**
     * @brief Command buffer that contains a sequence of rendering commands
     *
     * This buffer is API-agnostic and contains high-level rendering operations.
     * It gets executed by the RenderBackend using the appropriate HAL device.
     */
    class CommandBuffer {
    private:
        std::vector<std::unique_ptr<DrawCommand>> commands;

    public:
        CommandBuffer() = default;
        ~CommandBuffer() = default;

        // Move-only semantics
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;
        CommandBuffer(CommandBuffer&&) = default;
        CommandBuffer& operator=(CommandBuffer&&) = default;

        /**
         * @brief Add a clear command
         */
        void clear(float r, float g, float b, float a) {
            commands.emplace_back(std::make_unique<ClearCommand>(r, g, b, a));
        }

        /**
         * @brief Add a colored quad draw command
         */
        void drawQuad(const math::Mat4& modelMatrix, float r, float g, float b, float a) {
            commands.emplace_back(std::make_unique<DrawQuadCommand>(modelMatrix, r, g, b, a));
        }

        /**
         * @brief Add a textured quad draw command
         */
        void drawTexturedQuad(const math::Mat4& modelMatrix, uint32_t textureId) {
            commands.emplace_back(std::make_unique<DrawTexturedQuadCommand>(modelMatrix, textureId));
        }

        /**
         * @brief Add a viewport command
         */
        void setViewport(int x, int y, int width, int height) {
            commands.emplace_back(std::make_unique<ViewportCommand>(x, y, width, height));
        }

        /**
         * @brief Add a projection matrix command
         */
        void setProjectionMatrix(const math::Mat4& matrix) {
            commands.emplace_back(std::make_unique<ProjectionMatrixCommand>(matrix));
        }

        /**
         * @brief Add a view matrix command
         */
        void setViewMatrix(const math::Mat4& matrix) {
            commands.emplace_back(std::make_unique<ViewMatrixCommand>(matrix));
        }

        /**
         * @brief Get all commands for execution
         */
        const std::vector<std::unique_ptr<DrawCommand>>& getCommands() const {
            return commands;
        }

        /**
         * @brief Clear all commands
         */
        void clear() {
            commands.clear();
        }

        /**
         * @brief Check if buffer is empty
         */
        bool empty() const {
            return commands.empty();
        }

        /**
         * @brief Get number of commands
         */
        size_t size() const {
            return commands.size();
        }
    };

} // namespace rendering::backend

#pragma once

#include "../../math/math.h"
#include <vector>
#include <string>
#include <memory>

namespace scene {

    /**
     * @brief Basic draw item for rendering
     */
    struct DrawItem {
        math::Matrix4f modelMatrix{ 1.0f };
        std::string materialId;
        std::string meshId;
        uint32_t renderLayer = 0;
        float depth = 0.0f;

        DrawItem() = default;
        DrawItem(const math::Matrix4f& matrix, const std::string& material,
            const std::string& mesh, uint32_t layer = 0)
            : modelMatrix(matrix), materialId(material), meshId(mesh), renderLayer(layer) {}
    };

    /**
     * @brief Post-processing effect description
     */
    struct PostEffect {
        std::string effectId;
        std::vector<float> parameters;
        bool enabled = true;

        PostEffect() = default;
        PostEffect(const std::string& id, bool enable = true)
            : effectId(id), enabled(enable) {}
    };

    /**
     * @brief UI rendering item
     */
    struct UIItem {
        math::Vec2f position{ 0.0f };
        math::Vec2f size{ 1.0f };
        std::string textureId;
        math::Vec4f color{ 1.0f };
        uint32_t layer = 0;

        UIItem() = default;
        UIItem(const math::Vec2f& pos, const math::Vec2f& sz, const std::string& tex)
            : position(pos), size(sz), textureId(tex) {}
    };

    /**
     * @brief Camera parameters for rendering
     */
    struct CameraParams {
        math::Matrix4f viewMatrix{ 1.0f };
        math::Matrix4f projectionMatrix{ 1.0f };
        math::Vec3f position{ 0.0f };
        math::Vec3f forward{ 0.0f, 0.0f, -1.0f };
        math::Vec3f up{ 0.0f, 1.0f, 0.0f };
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;
        float fov = 45.0f;

        CameraParams() = default;
    };

    /**
     * @brief Render target specification
     */
    struct RenderTarget {
        uint32_t width = 1920;
        uint32_t height = 1080;
        std::string targetId = "default";
        bool isBackbuffer = true;

        RenderTarget() = default;
        RenderTarget(uint32_t w, uint32_t h, const std::string& id = "default")
            : width(w), height(h), targetId(id), isBackbuffer(false) {}
    };

    /**
     * @brief Command buffer containing all rendering commands for a frame
     */
    class CommandBuffer {
    private:
        std::vector<DrawItem> drawItems;
        std::vector<PostEffect> postEffects;
        std::vector<UIItem> uiItems;
        CameraParams camera;
        RenderTarget target;

    public:
        /**
         * @brief Add a draw item
         */
        void addDrawItem(const DrawItem& item) {
            drawItems.push_back(item);
        }

        /**
         * @brief Add a post effect
         */
        void addPostEffect(const PostEffect& effect) {
            postEffects.push_back(effect);
        }

        /**
         * @brief Add a UI item
         */
        void addUIItem(const UIItem& item) {
            uiItems.push_back(item);
        }

        /**
         * @brief Set camera parameters
         */
        void setCamera(const CameraParams& cameraParams) {
            camera = cameraParams;
        }

        /**
         * @brief Set render target
         */
        void setRenderTarget(const RenderTarget& renderTarget) {
            target = renderTarget;
        }

        /**
         * @brief Clear all commands
         */
        void clear() {
            drawItems.clear();
            postEffects.clear();
            uiItems.clear();
        }

        /**
         * @brief Get draw items
         */
        const std::vector<DrawItem>& getDrawItems() const { return drawItems; }

        /**
         * @brief Get post effects
         */
        const std::vector<PostEffect>& getPostEffects() const { return postEffects; }

        /**
         * @brief Get UI items
         */
        const std::vector<UIItem>& getUIItems() const { return uiItems; }

        /**
         * @brief Get camera parameters
         */
        const CameraParams& getCamera() const { return camera; }

        /**
         * @brief Get render target
         */
        const RenderTarget& getRenderTarget() const { return target; }

        /**
         * @brief Get total command count
         */
        size_t getTotalCommandCount() const {
            return drawItems.size() + postEffects.size() + uiItems.size();
        }
    };

} // namespace scene

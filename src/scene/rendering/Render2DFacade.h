#pragma once

#include "Renderer2D.h"
#include "../../math/math.h"
#include <vector>
#include <string>

namespace scene {

    /**
     * @brief Unified facade for 2D rendering requests
     *
     * This facade centralizes the logic for collecting and submitting 2D rendering commands,
     * removing duplication between ECS-based systems and standalone test environments.
     * It replaces the need for TestRenderer2D while maintaining compatibility with both contexts.
     */
    class Render2DFacade {
    public:
        /**
         * @brief A single draw request for 2D rendering
         *
         * This structure contains all the data needed to render a 2D quad/sprite.
         * It centralizes the conversion logic that was duplicated between TestRenderer2D and Renderer2DSystem.
         */
        struct DrawRequest {
            Rect2D rect;                    // Position and size
            Color color;                    // RGBA color
            TextureHandle textureId;        // Texture handle (empty for colored quads)
            uint32_t layer = 0;            // Rendering layer for sorting
            float rotation = 0.0f;         // Rotation in radians
            float depth = 0.0f;            // Depth for Z-sorting

            // Constructors for convenience
            DrawRequest() = default;

            DrawRequest(const Rect2D& r, const Color& c, const TextureHandle& tex = "",
                uint32_t renderLayer = 0, float rot = 0.0f, float d = 0.0f)
                : rect(r), color(c), textureId(tex), layer(renderLayer), rotation(rot), depth(d) {}

            // Helper factory methods
            static DrawRequest createColoredQuad(const math::Vec2f& position, const math::Vec2f& size,
                const Color& color, uint32_t layer = 0, float rotation = 0.0f) {
                return DrawRequest(Rect2D(position, size), color, "", layer, rotation, 0.0f);
            }

            static DrawRequest createSprite(const math::Vec2f& position, const math::Vec2f& size,
                const TextureHandle& texture, const Color& color = Color::White,
                uint32_t layer = 0, float rotation = 0.0f) {
                return DrawRequest(Rect2D(position, size), color, texture, layer, rotation, 0.0f);
            }
        };

    private:
        std::vector<DrawRequest> drawRequests;

        // Statistics
        mutable uint32_t lastFrameQuadCount = 0;
        mutable uint32_t totalFramesRendered = 0;

    public:
        /**
         * @brief Default constructor
         */
        Render2DFacade() = default;

        /**
         * @brief Submit a draw request to the facade
         * @param request The draw request to submit
         */
        void submit(const DrawRequest& request) {
            drawRequests.push_back(request);
        }

        /**
         * @brief Submit a draw request with individual parameters
         * @param rect Rectangle to draw
         * @param color Color of the rectangle
         * @param textureId Optional texture handle
         * @param layer Rendering layer (for sorting)
         * @param rotation Rotation in radians
         * @param depth Depth for Z-sorting
         */
        void submit(const Rect2D& rect, const Color& color, const TextureHandle& textureId = "",
            uint32_t layer = 0, float rotation = 0.0f, float depth = 0.0f) {
            submit(DrawRequest(rect, color, textureId, layer, rotation, depth));
        }

        /**
         * @brief Flush all pending draw requests to the renderer
         * @param renderer The IRenderer2D instance to render with
         * @param camera The camera to use for this frame
         *
         * This method performs the actual rendering by:
         * 1. Beginning a scene with the provided camera
         * 2. Submitting all pending draw requests to the renderer
         * 3. Ending the scene
         * 4. Clearing the pending requests for the next frame
         */
        void flush(IRenderer2D& renderer, const Camera2D& camera) {
            if (drawRequests.empty()) {
                return;
            }

            // Begin the rendering scene
            renderer.beginScene(camera);

            // Submit all draw requests
            for (const auto& request : drawRequests) {
                renderer.drawRect(
                    request.rect,
                    request.color,
                    request.rotation,
                    request.textureId,
                    request.layer,
                    request.depth
                );
            }

            // End the scene (this triggers the actual rendering/batching)
            renderer.endScene();

            // Update statistics
            lastFrameQuadCount = static_cast<uint32_t>(drawRequests.size());
            totalFramesRendered++;

            // Clear requests for next frame
            clear();
        }

        /**
         * @brief Clear all pending draw requests
         *
         * This is automatically called by flush(), but can be called manually
         * if you need to cancel pending requests.
         */
        void clear() {
            drawRequests.clear();
        }

        /**
         * @brief Get the number of pending draw requests
         * @return Number of requests waiting to be flushed
         */
        size_t getPendingRequestCount() const {
            return drawRequests.size();
        }

        /**
         * @brief Get the number of quads rendered in the last frame
         * @return Number of quads rendered in the last flush() call
         */
        uint32_t getLastFrameQuadCount() const {
            return lastFrameQuadCount;
        }

        /**
         * @brief Get the total number of frames rendered through this facade
         * @return Total number of flush() calls
         */
        uint32_t getTotalFramesRendered() const {
            return totalFramesRendered;
        }

        /**
         * @brief Check if there are pending draw requests
         * @return true if there are requests waiting to be flushed
         */
        bool hasPendingRequests() const {
            return !drawRequests.empty();
        }

        // Utility methods for common entity creation patterns

        /**
         * @brief Create and submit a colored quad request
         * @param position Quad position
         * @param size Quad size
         * @param color Quad color
         * @param layer Rendering layer
         * @param rotation Rotation in radians
         */
        void submitColoredQuad(const math::Vec2f& position, const math::Vec2f& size,
            const Color& color, uint32_t layer = 0, float rotation = 0.0f) {
            submit(DrawRequest::createColoredQuad(position, size, color, layer, rotation));
        }

        /**
         * @brief Create and submit a sprite request
         * @param position Sprite position
         * @param size Sprite size
         * @param texture Texture handle
         * @param color Sprite tint color
         * @param layer Rendering layer
         * @param rotation Rotation in radians
         */
        void submitSprite(const math::Vec2f& position, const math::Vec2f& size,
            const TextureHandle& texture, const Color& color = Color::White,
            uint32_t layer = 0, float rotation = 0.0f) {
            submit(DrawRequest::createSprite(position, size, texture, color, layer, rotation));
        }
    };

} // namespace scene

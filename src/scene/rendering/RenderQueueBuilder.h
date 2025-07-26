#pragma once

#include "CommandBuffer.h"
#include <algorithm>

namespace scene {

    /**
     * @brief Helper class for building render queues and command buffers
     *
     * This class receives DrawItems, PostEffects, and UIItems from systems
     * or scene traversal and builds a CommandBuffer independent of the backend.
     */
    class RenderQueueBuilder {
    private:
        std::vector<DrawItem> drawItems;
        std::vector<PostEffect> postEffects;
        std::vector<UIItem> uiItems;

    public:
        /**
         * @brief Enqueue a draw item
         */
        void enqueue(const DrawItem& item) {
            drawItems.push_back(item);
        }

        /**
         * @brief Enqueue multiple draw items
         */
        void enqueue(const std::vector<DrawItem>& items) {
            drawItems.insert(drawItems.end(), items.begin(), items.end());
        }

        /**
         * @brief Enqueue a post effect
         */
        void enqueue(const PostEffect& effect) {
            postEffects.push_back(effect);
        }

        /**
         * @brief Enqueue multiple post effects
         */
        void enqueue(const std::vector<PostEffect>& effects) {
            postEffects.insert(postEffects.end(), effects.begin(), effects.end());
        }

        /**
         * @brief Enqueue a UI item
         */
        void enqueue(const UIItem& item) {
            uiItems.push_back(item);
        }

        /**
         * @brief Enqueue multiple UI items
         */
        void enqueue(const std::vector<UIItem>& items) {
            uiItems.insert(uiItems.end(), items.begin(), items.end());
        }

        /**
         * @brief Clear all queued items
         */
        void clear() {
            drawItems.clear();
            postEffects.clear();
            uiItems.clear();
        }

        /**
         * @brief Flush queues and build command buffer
         * @param camera Camera parameters for rendering
         * @param target Render target specification
         * @return Built command buffer
         */
        CommandBuffer flush(const CameraParams& camera, const RenderTarget& target) {
            CommandBuffer commandBuffer;

            // Set camera and target
            commandBuffer.setCamera(camera);
            commandBuffer.setRenderTarget(target);

            // Sort draw items by layer and depth
            sortDrawItems();

            // Add all items to command buffer
            for (const auto& item : drawItems) {
                commandBuffer.addDrawItem(item);
            }

            for (const auto& effect : postEffects) {
                commandBuffer.addPostEffect(effect);
            }

            // Sort UI items by layer
            sortUIItems();
            for (const auto& item : uiItems) {
                commandBuffer.addUIItem(item);
            }

            // Clear queues after flushing
            clear();

            return commandBuffer;
        }

        /**
         * @brief Get current queue sizes
         */
        struct QueueSizes {
            size_t drawItems = 0;
            size_t postEffects = 0;
            size_t uiItems = 0;
        };

        QueueSizes getQueueSizes() const {
            return { drawItems.size(), postEffects.size(), uiItems.size() };
        }

        /**
         * @brief Check if queues are empty
         */
        bool isEmpty() const {
            return drawItems.empty() && postEffects.empty() && uiItems.empty();
        }

    private:
        /**
         * @brief Sort draw items by layer and depth
         */
        void sortDrawItems() {
            std::sort(drawItems.begin(), drawItems.end(),
                [](const DrawItem& a, const DrawItem& b) {
                    if (a.renderLayer != b.renderLayer) {
                        return a.renderLayer < b.renderLayer;
                    }
                    return a.depth < b.depth; // Front to back for opaque objects
                });
        }

        /**
         * @brief Sort UI items by layer
         */
        void sortUIItems() {
            std::sort(uiItems.begin(), uiItems.end(),
                [](const UIItem& a, const UIItem& b) {
                    return a.layer < b.layer;
                });
        }
    };

} // namespace scene

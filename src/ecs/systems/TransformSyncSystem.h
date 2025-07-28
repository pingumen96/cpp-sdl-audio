#pragma once

#include "../System.h"
#include "../components/CommonComponents.h"
#include "../../scene/SceneNode.h"
#include "../../math/math.h"
#include <unordered_map>

namespace scene {

    /**
     * @brief System that synchronizes Transform components with SceneNode matrices
     *
     * This system ensures that SceneNode matrices reflect the authoritative
     * Transform component data. It runs once per frame and handles the
     * propagation of transformations through the hierarchy.
     */
    class TransformSyncSystem : public ecs::System {
    private:
        // Map from entities to their corresponding scene nodes
        std::unordered_map<ecs::Entity, SceneNode*> entityToNode;

        // Root nodes (nodes without parents in the scene hierarchy)
        std::vector<SceneNode*> rootNodes;

        // Reference to coordinator for accessing Transform components
        ecs::Coordinator* coordinator = nullptr;

    public:
        /**
         * @brief Register a scene node with its linked entity
         * @param entity ECS entity (must have Transform component)
         * @param node Scene node to sync with
         */
        void registerNode(ecs::Entity entity, SceneNode* node) {
            if (node) {
                entityToNode[entity] = node;

                // If this node has no parent, it's a root node
                if (!node->getParent()) {
                    auto it = std::find(rootNodes.begin(), rootNodes.end(), node);
                    if (it == rootNodes.end()) {
                        rootNodes.push_back(node);
                    }
                }
            }
        }

        /**
         * @brief Unregister a scene node
         * @param entity ECS entity to unregister
         */
        void unregisterNode(ecs::Entity entity) {
            auto it = entityToNode.find(entity);
            if (it != entityToNode.end()) {
                SceneNode* node = it->second;

                // Remove from root nodes if present
                auto rootIt = std::find(rootNodes.begin(), rootNodes.end(), node);
                if (rootIt != rootNodes.end()) {
                    rootNodes.erase(rootIt);
                }

                entityToNode.erase(it);
            }
        }

        /**
         * @brief Add a root node to the system
         * @param node Root node to add
         */
        void addRootNode(SceneNode* node) {
            if (node && std::find(rootNodes.begin(), rootNodes.end(), node) == rootNodes.end()) {
                rootNodes.push_back(node);
            }
        }

        /**
         * @brief Remove a root node from the system
         * @param node Root node to remove
         */
        void removeRootNode(SceneNode* node) {
            auto it = std::find(rootNodes.begin(), rootNodes.end(), node);
            if (it != rootNodes.end()) {
                rootNodes.erase(it);
            }
        }

        /**
         * @brief Update system - synchronize all transforms
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override {
            // Process all root nodes and their hierarchies
            for (SceneNode* rootNode : rootNodes) {
                updateNodeHierarchy(rootNode, math::Matrix4f::identity());
            }
        }

        /**
         * @brief Set the coordinator reference for accessing Transform components
         * @param coord Pointer to the ECS coordinator
         */
        void setCoordinator(ecs::Coordinator* coord) {
            coordinator = coord;
        }

    private:
        /**
         * @brief Convert ecs::components::Transform to math::Matrix4f
         */
        math::Matrix4f getTransformMatrix(const ecs::components::Transform& transform) {
            math::Matrix4f result = math::Matrix4f::identity();

            // Apply translation
            result = math::translate(result, transform.position);

            // Apply rotation from quaternion
            math::Matrix4f rotationMatrix = transform.rotation.toMatrix();
            result = result * rotationMatrix;

            // Apply scale
            result = math::scale(result, transform.scale);

            return result;
        }

    public:
        /**
         * @brief Get registered entity count
         */
        size_t getRegisteredEntityCount() const {
            return entityToNode.size();
        }

        /**
         * @brief Get root node count
         */
        size_t getRootNodeCount() const {
            return rootNodes.size();
        }

        /**
         * @brief Get scene node for entity
         * @param entity Entity to look up
         * @return Pointer to scene node or nullptr if not found
         */
        SceneNode* getNodeForEntity(ecs::Entity entity) const {
            auto it = entityToNode.find(entity);
            return (it != entityToNode.end()) ? it->second : nullptr;
        }

        /**
         * @brief Clear all registrations
         */
        void clear() {
            entityToNode.clear();
            rootNodes.clear();
        }

    private:
        /**
         * @brief Recursively update node hierarchy
         * @param node Current node to update
         * @param parentWorldMatrix World matrix of parent node
         */
        void updateNodeHierarchy(SceneNode* node, const math::Matrix4f& parentWorldMatrix) {
            if (!node) return;

            // Update local matrix from Transform component if entity is linked
            if (node->hasEntity() && coordinator) {
                ecs::Entity entity = node->getEntity().value();

                // Check if entity has Transform component and update if dirty
                if (coordinator->hasComponent<ecs::components::Transform>(entity) && node->isTransformDirty()) {
                    auto& transform = coordinator->getComponent<ecs::components::Transform>(entity);
                    node->updateLocalMatrix(getTransformMatrix(transform));
                }
            }

            // Calculate world matrix
            math::Matrix4f worldMatrix = parentWorldMatrix * node->getLocalMatrix();
            node->updateWorldMatrix(worldMatrix);

            // Recursively update children
            for (const auto& child : node->getChildren()) {
                updateNodeHierarchy(child.get(), worldMatrix);
            }
        }
    };

} // namespace scene

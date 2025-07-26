#pragma once

#include "../../ecs/System.h"
#include "../../ecs/components/CommonComponents.h"
#include "../SceneNode.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
                updateNodeHierarchy(rootNode, glm::mat4(1.0f));
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
         * @brief Convert ecs::components::Transform to GLM matrix
         */
        glm::mat4 getTransformMatrix(const ecs::components::Transform& transform) {
            glm::mat4 result = glm::mat4(1.0f);

            // Apply translation
            result = glm::translate(result, glm::vec3(transform.position[0], transform.position[1], transform.position[2]));

            // Apply rotation (assuming XYZ order)
            result = glm::rotate(result, transform.rotation[0], glm::vec3(1, 0, 0));
            result = glm::rotate(result, transform.rotation[1], glm::vec3(0, 1, 0));
            result = glm::rotate(result, transform.rotation[2], glm::vec3(0, 0, 1));

            // Apply scale
            result = glm::scale(result, glm::vec3(transform.scale[0], transform.scale[1], transform.scale[2]));

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
        void updateNodeHierarchy(SceneNode* node, const glm::mat4& parentWorldMatrix) {
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
            glm::mat4 worldMatrix = parentWorldMatrix * node->getLocalMatrix();
            node->updateWorldMatrix(worldMatrix);

            // Recursively update children
            for (const auto& child : node->getChildren()) {
                updateNodeHierarchy(child.get(), worldMatrix);
            }
        }
    };

} // namespace scene

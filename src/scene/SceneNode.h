#pragma once

#include "SceneTypes.h"
#include "../ecs/ECS.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <optional>

namespace scene {

    /**
     * @brief Hierarchical scene node with optional ECS entity link
     *
     * SceneNode is a thin adapter that provides hierarchical organization
     * with optional links to ECS entities. The authoritative transform data
     * lives in the entity's Transform component; the node's matrices are
     * synchronized by TransformSyncSystem.
     */
    class SceneNode {
    private:
        // Hierarchy
        SceneNode* parent = nullptr;
        std::vector<std::unique_ptr<SceneNode>> children;

        // Transform matrices (synchronized from ECS Transform component)
        glm::mat4 localMatrix{ 1.0f };
        glm::mat4 worldMatrix{ 1.0f };

        // Optional ECS entity link
        std::optional<ecs::Entity> entity;

        // Node metadata
        std::string name;
        bool visible = true;
        bool transformDirty = true;

    public:
        /**
         * @brief Constructor for node without entity
         */
        explicit SceneNode(const std::string& nodeName = "")
            : name(nodeName) {}

        /**
         * @brief Constructor for node with linked entity
         */
        SceneNode(ecs::Entity linkedEntity, const std::string& nodeName = "")
            : entity(linkedEntity), name(nodeName) {}

        /**
         * @brief Destructor
         */
        ~SceneNode() = default;

        // Non-copyable but movable
        SceneNode(const SceneNode&) = delete;
        SceneNode& operator=(const SceneNode&) = delete;
        SceneNode(SceneNode&&) = default;
        SceneNode& operator=(SceneNode&&) = default;

        /**
         * @brief Add a child node
         * @param child Unique pointer to child node
         * @return Raw pointer to the added child for convenience
         */
        SceneNode* addChild(std::unique_ptr<SceneNode> child) {
            if (child) {
                child->parent = this;
                child->markTransformDirty();
                SceneNode* rawPtr = child.get();
                children.push_back(std::move(child));
                return rawPtr;
            }
            return nullptr;
        }

        /**
         * @brief Remove a child node
         * @param child Pointer to child to remove
         * @return Unique pointer to removed child (for reparenting)
         */
        std::unique_ptr<SceneNode> removeChild(SceneNode* child) {
            auto it = std::find_if(children.begin(), children.end(),
                [child](const std::unique_ptr<SceneNode>& ptr) {
                    return ptr.get() == child;
                });

            if (it != children.end()) {
                std::unique_ptr<SceneNode> removedChild = std::move(*it);
                children.erase(it);
                removedChild->parent = nullptr;
                removedChild->markTransformDirty();
                return removedChild;
            }
            return nullptr;
        }

        /**
         * @brief Visit this node and all children with a visitor function
         * @param visitor Function to call for each node
         * @param visitSelf Whether to visit this node (default: true)
         */
        void visit(const std::function<void(SceneNode*)>& visitor, bool visitSelf = true) {
            if (visitSelf) {
                visitor(this);
            }

            for (auto& child : children) {
                child->visit(visitor, true);
            }
        }

        /**
         * @brief Visit this node and all children (const version)
         */
        void visitConst(const std::function<void(const SceneNode*)>& visitor, bool visitSelf = true) const {
            if (visitSelf) {
                visitor(this);
            }

            for (const auto& child : children) {
                child->visitConst(visitor, true);
            }
        }

        /**
         * @brief Find first child with given name
         * @param childName Name to search for
         * @param recursive Whether to search recursively
         * @return Pointer to found child or nullptr
         */
        SceneNode* findChild(const std::string& childName, bool recursive = false) {
            for (auto& child : children) {
                if (child->name == childName) {
                    return child.get();
                }
                if (recursive) {
                    SceneNode* found = child->findChild(childName, true);
                    if (found) return found;
                }
            }
            return nullptr;
        }

        /**
         * @brief Find first child with given name (const version)
         */
        const SceneNode* findChild(const std::string& childName, bool recursive = false) const {
            for (const auto& child : children) {
                if (child->name == childName) {
                    return child.get();
                }
                if (recursive) {
                    const SceneNode* found = child->findChild(childName, true);
                    if (found) return found;
                }
            }
            return nullptr;
        }

        // Getters and setters

        /**
         * @brief Get parent node
         */
        SceneNode* getParent() const { return parent; }

        /**
         * @brief Get children (read-only access)
         */
        const std::vector<std::unique_ptr<SceneNode>>& getChildren() const { return children; }

        /**
         * @brief Get child count
         */
        size_t getChildCount() const { return children.size(); }

        /**
         * @brief Get linked entity (if any)
         */
        std::optional<ecs::Entity> getEntity() const { return entity; }

        /**
         * @brief Set linked entity
         */
        void setEntity(ecs::Entity newEntity) {
            entity = newEntity;
            markTransformDirty();
        }

        /**
         * @brief Clear linked entity
         */
        void clearEntity() {
            entity.reset();
        }

        /**
         * @brief Check if node has linked entity
         */
        bool hasEntity() const { return entity.has_value(); }

        /**
         * @brief Get node name
         */
        const std::string& getName() const { return name; }

        /**
         * @brief Set node name
         */
        void setName(const std::string& newName) { name = newName; }

        /**
         * @brief Get visibility
         */
        bool isVisible() const { return visible; }

        /**
         * @brief Set visibility
         */
        void setVisible(bool isVisible) { visible = isVisible; }

        /**
         * @brief Get local transformation matrix
         */
        const glm::mat4& getLocalMatrix() const { return localMatrix; }

        /**
         * @brief Get world transformation matrix
         */
        const glm::mat4& getWorldMatrix() const { return worldMatrix; }

        /**
         * @brief Update local matrix (called by TransformSyncSystem)
         * @param matrix New local transformation matrix
         */
        void updateLocalMatrix(const glm::mat4& matrix) {
            localMatrix = matrix;
            markTransformDirty();
        }

        /**
         * @brief Update world matrix (called by TransformSyncSystem)
         * @param matrix New world transformation matrix
         */
        void updateWorldMatrix(const glm::mat4& matrix) {
            worldMatrix = matrix;
            transformDirty = false;
        }

        /**
         * @brief Check if transform needs update
         */
        bool isTransformDirty() const { return transformDirty; }

        /**
         * @brief Mark transform as needing update
         */
        void markTransformDirty() {
            transformDirty = true;
            // Mark all children as dirty too
            for (auto& child : children) {
                child->markTransformDirty();
            }
        }

        /**
         * @brief Get depth in hierarchy (root = 0)
         */
        size_t getDepth() const {
            size_t depth = 0;
            const SceneNode* current = parent;
            while (current) {
                depth++;
                current = current->parent;
            }
            return depth;
        }

        /**
         * @brief Check if this node is ancestor of another node
         */
        bool isAncestorOf(const SceneNode* other) const {
            if (!other) return false;

            const SceneNode* current = other->parent;
            while (current) {
                if (current == this) return true;
                current = current->parent;
            }
            return false;
        }

        /**
         * @brief Get root node of the hierarchy
         */
        SceneNode* getRoot() {
            SceneNode* current = this;
            while (current->parent) {
                current = current->parent;
            }
            return current;
        }

        /**
         * @brief Get root node of the hierarchy (const version)
         */
        const SceneNode* getRoot() const {
            const SceneNode* current = this;
            while (current->parent) {
                current = current->parent;
            }
            return current;
        }
    };

} // namespace scene

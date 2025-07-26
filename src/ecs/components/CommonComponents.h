#pragma once

#include "../../math/math.h"

namespace ecs::components {

    /**
     * @brief Transform component for position, rotation, and scale
     */
    struct Transform {
        math::Vec3f position{ 0.0f, 0.0f, 0.0f };
        math::Quatf rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // w, x, y, z (identity)
        math::Vec3f scale{ 1.0f, 1.0f, 1.0f };

        /**
         * @brief Default constructor
         */
        Transform() = default;

        /**
         * @brief Constructor with position
         */
        Transform(const math::Vec3f& pos) : position(pos) {}

        /**
         * @brief Constructor with position and rotation
         */
        Transform(const math::Vec3f& pos, const math::Quatf& rot)
            : position(pos), rotation(rot) {}

        /**
         * @brief Constructor with all parameters
         */
        Transform(const math::Vec3f& pos, const math::Quatf& rot, const math::Vec3f& sc)
            : position(pos), rotation(rot), scale(sc) {}

        /**
         * @brief Calculate the local transformation matrix
         * @return Local transformation matrix
         */
        math::Matrix4f getMatrix() const {
            math::Matrix4f t = math::Matrix4f::translation(position);
            math::Matrix4f r = rotation.toMatrix();
            math::Matrix4f s = math::Matrix4f::scale(scale);
            return t * r * s;
        }

        /**
         * @brief Set rotation from euler angles (in radians)
         */
        void setEulerAngles(float x, float y, float z) {
            rotation = math::Quatf(math::Vec3f(x, y, z));
        }

        /**
         * @brief Get euler angles from rotation (in radians)
         */
        math::Vec3f getEulerAngles() const {
            return rotation.toEulerAngles();
        }

        /**
         * @brief Translate by offset
         */
        void translate(const math::Vec3f& offset) {
            position += offset;
        }

        /**
         * @brief Rotate by quaternion
         */
        void rotate(const math::Quatf& rot) {
            rotation = rot * rotation;
        }

        /**
         * @brief Scale uniformly
         */
        void scaleUniform(float factor) {
            scale *= factor;
        }
    };

    /**
     * @brief Velocity component for physics movement
     */
    struct Velocity {
        math::Vec3f linear{ 0.0f, 0.0f, 0.0f };   // Linear velocity
        math::Vec3f angular{ 0.0f, 0.0f, 0.0f };  // Angular velocity in radians/sec

        Velocity() = default;

        Velocity(const math::Vec3f& lin)
            : linear(lin) {}

        Velocity(const math::Vec3f& lin, const math::Vec3f& ang)
            : linear(lin), angular(ang) {}
    };

    /**
     * @brief Renderable component for visual objects
     */
    struct Renderable {
        std::uint32_t textureId{ 0 };  // OpenGL texture ID
        math::Vec4f color{ 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA color
        bool visible{ true };
        float layer{ 0.0f }; // Rendering layer/depth

        Renderable() = default;

        Renderable(std::uint32_t texId)
            : textureId(texId) {}

        Renderable(const math::Vec4f& col)
            : color(col) {}

        Renderable(std::uint32_t texId, const math::Vec4f& col)
            : textureId(texId), color(col) {}
    };

    /**
     * @brief Health component for entities that can take damage
     */
    struct Health {
        float current{ 100.0f };
        float maximum{ 100.0f };
        bool invulnerable{ false };

        Health() = default;

        Health(float max)
            : current(max), maximum(max) {}

        Health(float curr, float max)
            : current(curr), maximum(max) {}

        /**
         * @brief Check if entity is alive
         * @return true if current health > 0
         */
        bool isAlive() const {
            return current > 0.0f;
        }

        /**
         * @brief Get health percentage
         * @return Health as percentage (0.0 to 1.0)
         */
        float getHealthPercentage() const {
            return maximum > 0.0f ? current / maximum : 0.0f;
        }

        /**
         * @brief Apply damage to health
         * @param damage Damage amount (positive values reduce health)
         */
        void takeDamage(float damage) {
            if (!invulnerable) {
                current = std::max(0.0f, current - damage);
            }
        }

        /**
         * @brief Heal the entity
         * @param amount Healing amount (positive values increase health)
         */
        void heal(float amount) {
            current = std::min(maximum, current + amount);
        }
    };

    /**
     * @brief Tag component to identify player entities
     */
    struct PlayerTag {
        // Empty tag component - presence indicates this is a player entity
    };

    /**
     * @brief Tag component to identify enemy entities
     */
    struct EnemyTag {
        // Empty tag component - presence indicates this is an enemy entity
    };

} // namespace ecs::components

// Include additional component types
#include "Renderable2D.h"

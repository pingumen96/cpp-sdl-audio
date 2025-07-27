#pragma once

#include "../../math/math.h"

namespace scene {

    /**
     * @brief Transform component for spatial transformation data
     *
     * This is the authoritative source for entity positioning,
     * rotation, and scaling. SceneNode matrices are synchronized
     * from this component by the TransformSyncSystem.
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

} // namespace scene

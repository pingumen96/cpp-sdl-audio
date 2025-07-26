#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace scene {

    /**
     * @brief Transform component for spatial transformation data
     *
     * This is the authoritative source for entity positioning,
     * rotation, and scaling. SceneNode matrices are synchronized
     * from this component by the TransformSyncSystem.
     */
    struct Transform {
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // w, x, y, z (identity)
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

        /**
         * @brief Default constructor
         */
        Transform() = default;

        /**
         * @brief Constructor with position
         */
        Transform(const glm::vec3& pos) : position(pos) {}

        /**
         * @brief Constructor with position and rotation
         */
        Transform(const glm::vec3& pos, const glm::quat& rot)
            : position(pos), rotation(rot) {}

        /**
         * @brief Constructor with all parameters
         */
        Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& sc)
            : position(pos), rotation(rot), scale(sc) {}

        /**
         * @brief Calculate the local transformation matrix
         * @return Local transformation matrix
         */
        glm::mat4 getMatrix() const {
            glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 r = glm::mat4_cast(rotation);
            glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
            return t * r * s;
        }

        /**
         * @brief Set rotation from euler angles (in radians)
         */
        void setEulerAngles(float x, float y, float z) {
            rotation = glm::quat(glm::vec3(x, y, z));
        }

        /**
         * @brief Get euler angles from rotation (in radians)
         */
        glm::vec3 getEulerAngles() const {
            return glm::eulerAngles(rotation);
        }

        /**
         * @brief Translate by offset
         */
        void translate(const glm::vec3& offset) {
            position += offset;
        }

        /**
         * @brief Rotate by quaternion
         */
        void rotate(const glm::quat& rot) {
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

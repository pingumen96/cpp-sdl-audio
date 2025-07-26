#pragma once
// math/math.h – comprehensive math library header

// Include basic vector types first
#include "Vec.h"

// Include matrix types (needs Vec)
#include "Matrix.h"

// Include quaternion types (needs Vec and Matrix forward declaration)
#include "Quaternion.h"

// Include rectangle types
#include "Rect.h"

namespace math {
    /*================ GLM-compatible functions =================*/

    /**
     * @brief Convert quaternion to matrix (GLM mat4_cast equivalent)
     */
    template <Arithmetic T>
    Matrix4<T> mat4_cast(const Quaternion<T>& q) noexcept {
        return q.toMatrix();
    }

    /**
     * @brief Create a 4x4 identity matrix (GLM mat4(1.0f) equivalent)
     */
    template <Arithmetic T>
    constexpr Matrix4<T> mat4(T value) noexcept {
        return Matrix4<T>(value);
    }

    template <Arithmetic T>
    constexpr Matrix4<T> mat4() noexcept {
        return Matrix4<T>();
    }

    /**
     * @brief Create look-at matrix (GLM lookAt equivalent)
     */
    template <Arithmetic T>
    Matrix4<T> lookAt(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up) noexcept {
        return Matrix4<T>::lookAt(eye, center, up);
    }

    /**
     * @brief Create perspective projection matrix (GLM perspective equivalent)
     */
    template <Arithmetic T>
    Matrix4<T> perspective(T fovy, T aspect, T nearPlane, T farPlane) noexcept {
        return Matrix4<T>::perspective(fovy, aspect, nearPlane, farPlane);
    }
}

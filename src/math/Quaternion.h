#pragma once
// math/Quaternion.h – quaternion for rotations

#include "Vec.h"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace math {

    // Forward declaration
    template <Arithmetic T> struct Matrix4;

    // --- Quaternion<T> --------------------------------------------------------------
    template <Arithmetic T>
    struct Quaternion {
        using ValueType = T;

        T w, x, y, z;

        /*---------------- constructors ----------------*/
        constexpr Quaternion() noexcept : w(T(1)), x(T(0)), y(T(0)), z(T(0)) {}

        constexpr Quaternion(T w_, T x_, T y_, T z_) noexcept : w(w_), x(x_), y(y_), z(z_) {}

        // Construct from axis-angle
        Quaternion(T angle, const Vec3<T>& axis) noexcept {
            Vec3<T> normalizedAxis = normalize(axis);
            T halfAngle = angle / T(2);
            T s = std::sin(halfAngle);
            w = std::cos(halfAngle);
            x = normalizedAxis.x() * s;
            y = normalizedAxis.y() * s;
            z = normalizedAxis.z() * s;
        }

        // Construct from Euler angles (in radians)
        explicit Quaternion(const Vec3<T>& eulerAngles) noexcept {
            T cx = std::cos(eulerAngles.x() * T(0.5));
            T cy = std::cos(eulerAngles.y() * T(0.5));
            T cz = std::cos(eulerAngles.z() * T(0.5));
            T sx = std::sin(eulerAngles.x() * T(0.5));
            T sy = std::sin(eulerAngles.y() * T(0.5));
            T sz = std::sin(eulerAngles.z() * T(0.5));

            w = cx * cy * cz + sx * sy * sz;
            x = sx * cy * cz - cx * sy * sz;
            y = cx * sy * cz + sx * cy * sz;
            z = cx * cy * sz - sx * sy * cz;
        }

        /*---------------- operations ----------------*/
        constexpr Quaternion operator+(const Quaternion& other) const noexcept {
            return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
        }

        constexpr Quaternion operator-(const Quaternion& other) const noexcept {
            return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
        }

        constexpr Quaternion operator*(const Quaternion& other) const noexcept {
            return Quaternion(
                w * other.w - x * other.x - y * other.y - z * other.z,
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w
            );
        }

        constexpr Quaternion operator*(T scalar) const noexcept {
            return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
        }

        friend constexpr Quaternion operator*(T scalar, const Quaternion& q) noexcept {
            return q * scalar;
        }

        constexpr Quaternion& operator+=(const Quaternion& other) noexcept {
            w += other.w; x += other.x; y += other.y; z += other.z;
            return *this;
        }

        constexpr Quaternion& operator-=(const Quaternion& other) noexcept {
            w -= other.w; x -= other.x; y -= other.y; z -= other.z;
            return *this;
        }

        constexpr Quaternion& operator*=(const Quaternion& other) noexcept {
            *this = *this * other;
            return *this;
        }

        constexpr Quaternion& operator*=(T scalar) noexcept {
            w *= scalar; x *= scalar; y *= scalar; z *= scalar;
            return *this;
        }

        /*---------------- properties ----------------*/
        constexpr T dot(const Quaternion& other) const noexcept {
            return w * other.w + x * other.x + y * other.y + z * other.z;
        }

        constexpr T lengthSquared() const noexcept {
            return w * w + x * x + y * y + z * z;
        }

        T length() const noexcept {
            return std::sqrt(lengthSquared());
        }

        Quaternion normalized() const noexcept {
            T len = length();
            if (len > T(0)) {
                return *this * (T(1) / len);
            }
            return identity();
        }

        constexpr Quaternion conjugate() const noexcept {
            return Quaternion(w, -x, -y, -z);
        }

        Quaternion inverse() const noexcept {
            T lenSq = lengthSquared();
            if (lenSq > T(0)) {
                return conjugate() * (T(1) / lenSq);
            }
            return identity();
        }

        /*---------------- conversions ----------------*/
        Matrix4<T> toMatrix() const noexcept {
            T xx = x * x;
            T yy = y * y;
            T zz = z * z;
            T xy = x * y;
            T xz = x * z;
            T yz = y * z;
            T wx = w * x;
            T wy = w * y;
            T wz = w * z;

            Matrix4<T> result;
            result.m[0][0] = T(1) - T(2) * (yy + zz);
            result.m[1][0] = T(2) * (xy - wz);
            result.m[2][0] = T(2) * (xz + wy);
            result.m[3][0] = T(0);

            result.m[0][1] = T(2) * (xy + wz);
            result.m[1][1] = T(1) - T(2) * (xx + zz);
            result.m[2][1] = T(2) * (yz - wx);
            result.m[3][1] = T(0);

            result.m[0][2] = T(2) * (xz - wy);
            result.m[1][2] = T(2) * (yz + wx);
            result.m[2][2] = T(1) - T(2) * (xx + yy);
            result.m[3][2] = T(0);

            result.m[0][3] = T(0);
            result.m[1][3] = T(0);
            result.m[2][3] = T(0);
            result.m[3][3] = T(1);

            return result;
        }

        Vec3<T> toEulerAngles() const noexcept {
            // Roll (x-axis rotation)
            T sinr_cosp = T(2) * (w * x + y * z);
            T cosr_cosp = T(1) - T(2) * (x * x + y * y);
            T roll = std::atan2(sinr_cosp, cosr_cosp);

            // Pitch (y-axis rotation)
            T sinp = T(2) * (w * y - z * x);
            T pitch;
            if (std::abs(sinp) >= T(1)) {
                pitch = std::copysign(T(M_PI) / T(2), sinp); // Use 90 degrees if out of range
            } else {
                pitch = std::asin(sinp);
            }

            // Yaw (z-axis rotation)
            T siny_cosp = T(2) * (w * z + x * y);
            T cosy_cosp = T(1) - T(2) * (y * y + z * z);
            T yaw = std::atan2(siny_cosp, cosy_cosp);

            return Vec3<T>(roll, pitch, yaw);
        }

        /*---------------- static factory methods ----------------*/
        static constexpr Quaternion identity() noexcept {
            return Quaternion();
        }

        /*---------------- comparison ----------------*/
        constexpr bool operator==(const Quaternion& other) const noexcept {
            return w == other.w && x == other.x && y == other.y && z == other.z;
        }

        constexpr bool operator!=(const Quaternion& other) const noexcept {
            return !(*this == other);
        }

        /*---------------- stream output ----------------*/
        friend std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
            return os << "Quat(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
        }
    };

    /*================ global utility functions =================*/

    template <Arithmetic T>
    constexpr T dot(const Quaternion<T>& a, const Quaternion<T>& b) noexcept {
        return a.dot(b);
    }

    template <Arithmetic T>
    Quaternion<T> normalize(const Quaternion<T>& q) noexcept {
        return q.normalized();
    }

    template <Arithmetic T>
    Quaternion<T> slerp(const Quaternion<T>& a, const Quaternion<T>& b, T t) noexcept {
        T dotProduct = dot(a, b);

        // If the dot product is negative, slerp won't take the shorter path.
        // So negate one quaternion to correct this.
        Quaternion<T> end = b;
        if (dotProduct < T(0)) {
            end = Quaternion<T>(-b.w, -b.x, -b.y, -b.z);
            dotProduct = -dotProduct;
        }

        // If the inputs are too close for comfort, linearly interpolate
        if (dotProduct > T(0.9995)) {
            Quaternion<T> result = a + t * (end - a);
            return normalize(result);
        }

        // Since dotProduct is in range [0, 1], acos is safe
        T theta0 = std::acos(dotProduct);
        T theta = theta0 * t;
        T sinTheta = std::sin(theta);
        T sinTheta0 = std::sin(theta0);

        T s0 = std::cos(theta) - dotProduct * sinTheta / sinTheta0;
        T s1 = sinTheta / sinTheta0;

        return (s0 * a) + (s1 * end);
    }

    // Helper function to convert matrix to quaternion
    template <Arithmetic T>
    Quaternion<T> matrixToQuaternion(const Matrix4<T>& m) noexcept {
        T trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

        if (trace > T(0)) {
            T s = std::sqrt(trace + T(1)) * T(2); // s = 4 * qw
            return Quaternion<T>(
                s / T(4),
                (m.m[2][1] - m.m[1][2]) / s,
                (m.m[0][2] - m.m[2][0]) / s,
                (m.m[1][0] - m.m[0][1]) / s
            );
        } else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) {
            T s = std::sqrt(T(1) + m.m[0][0] - m.m[1][1] - m.m[2][2]) * T(2); // s = 4 * qx
            return Quaternion<T>(
                (m.m[2][1] - m.m[1][2]) / s,
                s / T(4),
                (m.m[0][1] + m.m[1][0]) / s,
                (m.m[0][2] + m.m[2][0]) / s
            );
        } else if (m.m[1][1] > m.m[2][2]) {
            T s = std::sqrt(T(1) + m.m[1][1] - m.m[0][0] - m.m[2][2]) * T(2); // s = 4 * qy
            return Quaternion<T>(
                (m.m[0][2] - m.m[2][0]) / s,
                (m.m[0][1] + m.m[1][0]) / s,
                s / T(4),
                (m.m[1][2] + m.m[2][1]) / s
            );
        } else {
            T s = std::sqrt(T(1) + m.m[2][2] - m.m[0][0] - m.m[1][1]) * T(2); // s = 4 * qz
            return Quaternion<T>(
                (m.m[1][0] - m.m[0][1]) / s,
                (m.m[0][2] + m.m[2][0]) / s,
                (m.m[1][2] + m.m[2][1]) / s,
                s / T(4)
            );
        }
    }

    /*================ handy aliases =================*/
    using Quaternionf = Quaternion<float>;
    using Quaterniond = Quaternion<double>;
    using Quatf = Quaternionf;
    using Quatd = Quaterniond;
    using Quat = Quaternionf; // Default to float

} // namespace math

/*================ hash specialisation =================*/
namespace std {
    template <math::Arithmetic T>
    struct hash<math::Quaternion<T>> {
        size_t operator()(const math::Quaternion<T>& q) const noexcept {
            size_t seed = 0x9e3779b97f4a7c15ULL;

            size_t h1 = hash<T>{}(q.w);
            size_t h2 = hash<T>{}(q.x);
            size_t h3 = hash<T>{}(q.y);
            size_t h4 = hash<T>{}(q.z);

            seed ^= h1 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= h4 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
} // namespace std

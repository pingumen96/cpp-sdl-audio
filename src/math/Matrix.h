#pragma once
// math/Matrix.h – 4x4 matrix for graphics transformations

#include "Vec.h"
#include <array>
#include <cmath>
#include <concepts>

namespace math {

    // --- Matrix4<T> --------------------------------------------------------------
    template <Arithmetic T>
    struct Matrix4 {
        using ValueType = T;
        static constexpr std::size_t Size = 4;

        // Column-major storage (OpenGL compatible)
        std::array<std::array<T, 4>, 4> m{};

        /*---------------- constructors ----------------*/
        constexpr Matrix4() noexcept {
            // Initialize as identity matrix
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    m[i][j] = (i == j) ? T(1) : T(0);
                }
            }
        }

        explicit constexpr Matrix4(T diagonal) noexcept {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    m[i][j] = (i == j) ? diagonal : T(0);
                }
            }
        }

        template <Arithmetic U>
        constexpr Matrix4(const Matrix4<U>& other) noexcept {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    m[i][j] = static_cast<T>(other.m[i][j]);
                }
            }
        }

        /*---------------- accessors ----------------*/
        constexpr T& operator()(int row, int col) noexcept {
            assert(row >= 0 && row < 4 && col >= 0 && col < 4);
            return m[col][row]; // Column-major
        }

        constexpr const T& operator()(int row, int col) const noexcept {
            assert(row >= 0 && row < 4 && col >= 0 && col < 4);
            return m[col][row]; // Column-major
        }

        constexpr std::array<T, 4>& operator[](int col) noexcept {
            assert(col >= 0 && col < 4);
            return m[col];
        }

        constexpr const std::array<T, 4>& operator[](int col) const noexcept {
            assert(col >= 0 && col < 4);
            return m[col];
        }

        // Get pointer to data for OpenGL
        constexpr const T* data() const noexcept {
            return &m[0][0];
        }

        /*---------------- matrix operations ----------------*/
        constexpr Matrix4 operator+(const Matrix4& other) const noexcept {
            Matrix4 result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.m[i][j] = m[i][j] + other.m[i][j];
                }
            }
            return result;
        }

        constexpr Matrix4 operator-(const Matrix4& other) const noexcept {
            Matrix4 result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.m[i][j] = m[i][j] - other.m[i][j];
                }
            }
            return result;
        }

        constexpr Matrix4 operator*(const Matrix4& other) const noexcept {
            Matrix4 result(T(0)); // Initialize with zeros
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        result.m[i][j] += m[k][j] * other.m[i][k];
                    }
                }
            }
            return result;
        }

        constexpr Matrix4 operator*(T scalar) const noexcept {
            Matrix4 result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.m[i][j] = m[i][j] * scalar;
                }
            }
            return result;
        }

        constexpr Matrix4& operator+=(const Matrix4& other) noexcept {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    m[i][j] += other.m[i][j];
                }
            }
            return *this;
        }

        constexpr Matrix4& operator-=(const Matrix4& other) noexcept {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    m[i][j] -= other.m[i][j];
                }
            }
            return *this;
        }

        constexpr Matrix4& operator*=(const Matrix4& other) noexcept {
            *this = *this * other;
            return *this;
        }

        constexpr Matrix4& operator*=(T scalar) noexcept {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    m[i][j] *= scalar;
                }
            }
            return *this;
        }

        /*---------------- vector transformation ----------------*/
        constexpr Vec4<T> operator*(const Vec4<T>& v) const noexcept {
            return Vec4<T>(
                m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
                m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
                m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
                m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
            );
        }

        /*---------------- matrix transformations ----------------*/
        constexpr Matrix4 transpose() const noexcept {
            Matrix4 result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.m[i][j] = m[j][i];
                }
            }
            return result;
        }

        // Static factory methods for common transformations
        static constexpr Matrix4 identity() noexcept {
            return Matrix4();
        }

        static constexpr Matrix4 translation(T x, T y, T z) noexcept {
            Matrix4 result;
            result.m[3][0] = x;
            result.m[3][1] = y;
            result.m[3][2] = z;
            return result;
        }

        static constexpr Matrix4 translation(const Vec3<T>& v) noexcept {
            return translation(v.x, v.y, v.z);
        }

        static constexpr Matrix4 scale(T x, T y, T z) noexcept {
            Matrix4 result(T(0));
            result.m[0][0] = x;
            result.m[1][1] = y;
            result.m[2][2] = z;
            result.m[3][3] = T(1);
            return result;
        }

        static constexpr Matrix4 scale(const Vec3<T>& v) noexcept {
            return scale(v.x, v.y, v.z);
        }

        static constexpr Matrix4 scale(T s) noexcept {
            return scale(s, s, s);
        }

        // Rotation around X axis
        static Matrix4 rotationX(T angle) noexcept {
            Matrix4 result;
            T c = std::cos(angle);
            T s = std::sin(angle);
            result.m[1][1] = c;
            result.m[1][2] = -s;
            result.m[2][1] = s;
            result.m[2][2] = c;
            return result;
        }

        // Rotation around Y axis
        static Matrix4 rotationY(T angle) noexcept {
            Matrix4 result;
            T c = std::cos(angle);
            T s = std::sin(angle);
            result.m[0][0] = c;
            result.m[0][2] = s;
            result.m[2][0] = -s;
            result.m[2][2] = c;
            return result;
        }

        // Rotation around Z axis
        static Matrix4 rotationZ(T angle) noexcept {
            Matrix4 result;
            T c = std::cos(angle);
            T s = std::sin(angle);
            result.m[0][0] = c;
            result.m[0][1] = s;
            result.m[1][0] = -s;
            result.m[1][1] = c;
            return result;
        }

        // Orthographic projection
        static constexpr Matrix4 ortho(T left, T right, T bottom, T top, T nearPlane, T farPlane) noexcept {
            Matrix4 result(T(0));
            result.m[0][0] = T(2) / (right - left);
            result.m[1][1] = T(2) / (top - bottom);
            result.m[2][2] = -T(2) / (farPlane - nearPlane);
            result.m[3][0] = -(right + left) / (right - left);
            result.m[3][1] = -(top + bottom) / (top - bottom);
            result.m[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
            result.m[3][3] = T(1);
            return result;
        }

        // Perspective projection
        static Matrix4 perspective(T fovy, T aspect, T nearPlane, T farPlane) noexcept {
            Matrix4 result(T(0));
            T tanHalfFovy = std::tan(fovy / T(2));

            result.m[0][0] = T(1) / (aspect * tanHalfFovy);
            result.m[1][1] = T(1) / tanHalfFovy;
            result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
            result.m[3][2] = -T(1);
            result.m[2][3] = -(T(2) * farPlane * nearPlane) / (farPlane - nearPlane);

            return result;
        }

        // Look-at matrix
        static Matrix4 lookAt(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up) noexcept {
            Vec3<T> f = normalize(center - eye);
            Vec3<T> s = normalize(cross(f, up));
            Vec3<T> u = cross(s, f);

            Matrix4 result;
            result.m[0][0] = s[0];
            result.m[1][0] = s[1];
            result.m[2][0] = s[2];
            result.m[0][1] = u[0];
            result.m[1][1] = u[1];
            result.m[2][1] = u[2];
            result.m[0][2] = -f[0];
            result.m[1][2] = -f[1];
            result.m[2][2] = -f[2];
            result.m[3][0] = -dot(s, eye);
            result.m[3][1] = -dot(u, eye);
            result.m[3][2] = dot(f, eye);

            return result;
        }
    };

    /*---------------- type aliases ----------------*/
    using Matrix4f = Matrix4<float>;
    using Matrix4d = Matrix4<double>;
    using Mat4f = Matrix4f;
    using Mat4d = Matrix4d;
    using Mat4 = Matrix4f; // Default to float

    /*---------------- comparison operators ----------------*/
    template <Arithmetic T>
    constexpr bool operator==(const Matrix4<T>& a, const Matrix4<T>& b) noexcept {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (a.m[i][j] != b.m[i][j]) return false;
            }
        }
        return true;
    }

    template <Arithmetic T>
    constexpr bool operator!=(const Matrix4<T>& a, const Matrix4<T>& b) noexcept {
        return !(a == b);
    }

    /*---------------- scalar multiplication ----------------*/
    template <Arithmetic T>
    constexpr Matrix4<T> operator*(T scalar, const Matrix4<T>& m) noexcept {
        return m * scalar;
    }

    /*---------------- stream output ----------------*/
    template <Arithmetic T>
    std::ostream& operator<<(std::ostream& os, const Matrix4<T>& m) {
        os << "Matrix4(\n";
        for (int i = 0; i < 4; ++i) {
            os << "  [";
            for (int j = 0; j < 4; ++j) {
                os << m(i, j);
                if (j < 3) os << ", ";
            }
            os << "]\n";
        }
        os << ")";
        return os;
    }

} // namespace math

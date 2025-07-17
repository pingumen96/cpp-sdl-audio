#pragma once

// generic 2D vector class, template on T with sum, difference, scalar
// multiplication, dot product, norm, and normalization
#include <cmath>
#include <concepts>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <cassert>
#include <algorithm>

namespace math {
    template <typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;


    template <Arithmetic T>
    struct Vec2 {
        using ValueType = T;

        T x{};
        T y{};

        constexpr Vec2() noexcept = default;
        constexpr Vec2(T x, T y) noexcept : x(x), y(y) {}
        constexpr explicit Vec2(T value) noexcept : x(value), y(value) {} // explicit scalar constructor

        template<Arithmetic U>
        constexpr Vec2(const Vec2<U>& other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

        constexpr T& operator[](std::size_t i) noexcept {
            assert(i < 2 && "Index out of bounds");
            return (i == 0) ? x : y;
        }
        constexpr const T& operator[](std::size_t i) const noexcept {
            assert(i < 2 && "Index out of bounds");
            return (i == 0) ? x : y;
        }

        constexpr Vec2& operator+=(const Vec2& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }
        constexpr Vec2& operator-=(const Vec2& other) noexcept {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        constexpr Vec2& operator*=(T scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return *this;
        }
        constexpr Vec2& operator/=(T scalar) noexcept {
            if (scalar == T{}) {
                throw std::domain_error("Division by zero");
            }

            x /= scalar;
            y /= scalar;
            return *this;
        }

        constexpr Vec2 operator+() const noexcept {
            return *this;
        }
        constexpr Vec2 operator-() const noexcept {
            return { -x, -y };
        }

        friend constexpr Vec2 operator+(Vec2 lhs, const Vec2& rhs) noexcept { return lhs += rhs; }
        friend constexpr Vec2 operator-(Vec2 lhs, const Vec2& rhs) noexcept { return lhs -= rhs; }
        friend constexpr Vec2 operator*(Vec2 lhs, T s) noexcept { return lhs *= s; }
        friend constexpr Vec2 operator*(T s, Vec2 rhs) noexcept { return rhs *= s; }
        friend constexpr Vec2 operator/(Vec2 lhs, T s) noexcept { return lhs /= s; }

        friend constexpr bool operator==(const Vec2&, const Vec2&) = default;
        friend constexpr auto operator<=>(const Vec2&, const Vec2&) = default;

        [[nodiscard]] constexpr T dot(const Vec2& other) const noexcept {
            return x * other.x + y * other.y;
        }

        [[nodiscard]] constexpr auto lengthSquared() const noexcept {
            return dot(*this);
        }

        [[nodiscard]] constexpr auto length() const noexcept {
            return std::sqrt(static_cast<std::common_type_t<T, double>>(lengthSquared()));
        }

        [[nodiscard]] constexpr Vec2 normalized() const noexcept {
            auto len = length();
            assert(len != T{} && "Cannot normalize zero-length vector");
            auto inv = T(1) / len;
            return { x * inv, y * inv };
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
            return os << "Vec2(" << vec.x << ", " << vec.y << ")";
        }

        static constexpr Vec2 zero() noexcept {
            return {};
        }

        static constexpr Vec2 one() noexcept {
            return { 1, 1 };
        }
    };


    // helpers
    using Vec2f = Vec2<float>;
    using Vec2d = Vec2<double>;
    using Vec2i = Vec2<int>;

    template <Arithmetic T>
    [[nodiscard]] constexpr auto cross(const Vec2<T>& a, const Vec2<T>& b) noexcept {
        return a.x * b.y - a.y * b.x;
    }

    template<Arithmetic T>
    [[nodiscard]] constexpr Vec2<T> clamp(const Vec2<T>& v,
        const Vec2<T>& min,
        const Vec2<T>& max) noexcept {
        return {
            std::clamp(v.x, min.x, max.x),
            std::clamp(v.y, min.y, max.y)
        };
    }

}  // namespace math

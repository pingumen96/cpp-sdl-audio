#pragma once
// math/vec.hpp – n-dimensional constexpr vector
#include <array>
#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include <functional>

namespace math {

    // --- concept ---------------------------------------------------------------
    template <typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    // --- Vec<N,T> --------------------------------------------------------------
    template <std::size_t N, Arithmetic T>
    struct Vec {
        using ValueType = T;
        static constexpr std::size_t Dim = N;

        std::array<T, N> data{};

        /*---------------- constructors ----------------*/
        constexpr Vec() noexcept = default;

        template <class... U>
            requires (sizeof...(U) == N) &&
        (std::is_convertible_v<U, T> && ...)
            explicit constexpr Vec(U... xs) noexcept : data{ static_cast<T>(xs)... } {}

        template <Arithmetic U>
        constexpr Vec(const Vec<N, U>& other) noexcept {
            for (std::size_t i = 0; i < N; ++i)
                data[i] = static_cast<T>(other[i]);
        }

        /*---------------- indexing ----------------*/
        constexpr       T& operator[](std::size_t i)       noexcept { assert(i < N); return data[i]; }
        constexpr const T& operator[](std::size_t i) const noexcept { assert(i < N); return data[i]; }

        /*---------------- arithmetic ----------------*/
        [[nodiscard]] constexpr Vec& operator+=(const Vec& o) noexcept {
            for (std::size_t i = 0; i < N; ++i) data[i] += o[i];
            return *this;
        }
        [[nodiscard]] constexpr Vec& operator-=(const Vec& o) noexcept {
            for (std::size_t i = 0; i < N; ++i) data[i] -= o[i];
            return *this;
        }
        [[nodiscard]] constexpr Vec& operator*=(T s) noexcept {
            for (auto& v : data) v *= s;
            return *this;
        }
        [[nodiscard]] constexpr Vec& operator/=(T s) {
            if (s == T{}) throw std::domain_error("Division by zero");
            for (auto& v : data) v /= s;
            return *this;
        }

        /*---------------- unary ----------------*/
        [[nodiscard]] constexpr Vec  operator+() const noexcept { return *this; }
        [[nodiscard]] constexpr Vec  operator-() const noexcept {
            Vec r{};
            for (std::size_t i = 0; i < N; ++i) r[i] = -data[i];
            return r;
        }

        /*---------------- binary free ops ----------------*/
        friend constexpr Vec operator+(Vec a, const Vec& b) noexcept { return a += b; }
        friend constexpr Vec operator-(Vec a, const Vec& b) noexcept { return a -= b; }
        friend constexpr Vec operator*(Vec a, T s)        noexcept { return a *= s; }
        friend constexpr Vec operator*(T s, Vec a)        noexcept { return a *= s; }
        friend constexpr Vec operator/(Vec a, T s) { return a /= s; }

        friend constexpr bool operator==(const Vec&, const Vec&) = default;
        friend constexpr auto operator<=>(const Vec&, const Vec&) = default;

        /*---------------- math helpers ----------------*/
        [[nodiscard]] constexpr T dot(const Vec& o) const noexcept {
            T acc{};
            for (std::size_t i = 0; i < N; ++i) acc += data[i] * o[i];
            return acc;
        }

        [[nodiscard]] constexpr auto lengthSquared() const noexcept { return dot(*this); }

        [[nodiscard]] constexpr auto length() const noexcept {
            return std::sqrt(static_cast<std::common_type_t<T, double>>(lengthSquared()));
        }

        [[nodiscard]] constexpr Vec normalized() const {
            auto len = length();
            assert(len != T{} && "Cannot normalize zero-length vector");
            return (*this) / static_cast<T>(len);
        }

        /*---------------- factories ----------------*/
        [[nodiscard]] static constexpr Vec zero() noexcept { return {}; }

        [[nodiscard]] static constexpr Vec one() noexcept {
            Vec r{};
            r.data.fill(T{ 1 });
            return r;
        }

        /*---------------- ostream ----------------*/
        friend std::ostream& operator<<(std::ostream& os, const Vec& v) {
            os << "Vec" << N << "(";
            for (std::size_t i = 0; i < N; ++i) {
                os << v[i] << (i + 1 < N ? ", " : "");
            }
            return os << ")";
        }
    };

    /*================ special utilities =================*/
    /* --- cross product --- */
    template <Arithmetic T>
    [[nodiscard]] constexpr T cross(const Vec<2, T>& a, const Vec<2, T>& b) noexcept {
        return a[0] * b[1] - a[1] * b[0];
    }

    template <Arithmetic T>
    [[nodiscard]] constexpr Vec<3, T> cross(const Vec<3, T>& a, const Vec<3, T>& b) noexcept {
        return {
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        };
    }

    /* --- clamp (component-wise) --- */
    template <std::size_t N, Arithmetic T>
    [[nodiscard]] constexpr Vec<N, T> clamp(const Vec<N, T>& v,
        const Vec<N, T>& min,
        const Vec<N, T>& max) noexcept {
        Vec<N, T> r{};
        for (std::size_t i = 0; i < N; ++i)
            r[i] = std::clamp(v[i], min[i], max[i]);
        return r;
    }

    /*================ handy aliases =================*/
    template <Arithmetic T> using Vec2 = Vec<2, T>;
    template <Arithmetic T> using Vec3 = Vec<3, T>;
    template <Arithmetic T> using Vec4 = Vec<4, T>;

    using Vec2f = Vec2<float>;
    using Vec3f = Vec3<float>;
    using Vec4f = Vec4<float>;

    using Vec2d = Vec2<double>;
    using Vec3d = Vec3<double>;
    using Vec4d = Vec4<double>;

    using Vec2i = Vec2<int>;
    using Vec3i = Vec3<int>;
    using Vec4i = Vec4<int>;

} // namespace math

/*================ hash specialisation =================*/
namespace std {
    template <size_t N, math::Arithmetic T>
    struct hash<math::Vec<N, T>> {
        size_t operator()(const math::Vec<N, T>& v) const noexcept {
            size_t seed = 0x9e3779b97f4a7c15ULL;
            for (size_t i = 0; i < N; ++i) {
                size_t h = hash<T>{}(v[i]);
                seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
} // namespace std

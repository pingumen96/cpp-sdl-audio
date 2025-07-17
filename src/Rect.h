#pragma once
// math/rect.hpp – generic axis-aligned box

#include "Vec.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <ostream>
#include <type_traits>

namespace math {

    template <Arithmetic T>
    struct Rect {
        Vec2<T> min{};   // bottom-left
        Vec2<T> max{};   // top-right

        /*----- helpers -----*/
    private:
        constexpr void canonicalize() noexcept {
            if (min.x > max.x) std::swap(min.x, max.x);
            if (min.y > max.y) std::swap(min.y, max.y);
        }

    public:
        /*-------------- costruttori / factory --------------*/
        constexpr Rect() noexcept = default;

        constexpr Rect(const Vec2<T>& mn, const Vec2<T>& mx) noexcept
            : min(mn), max(mx) {
            canonicalize();
        }

        [[nodiscard]] static constexpr Rect fromMinMax(Vec2<T> mn, Vec2<T> mx) {
            return Rect{ mn, mx };
        }

        [[nodiscard]] static constexpr Rect fromPosSize(Vec2<T> pos, Vec2<T> size) {
            return Rect{ pos, pos + size };
        }

        [[nodiscard]] static constexpr Rect fromCenterSize(Vec2<T> c, T w, T h) {
            Vec2<T> half{ w / T{2}, h / T{2} };
            return Rect{ c - half, c + half };
        }

        [[nodiscard]] static constexpr Rect<T> empty() noexcept { return Rect{}; }

        /*-------------- query --------------*/
        [[nodiscard]] constexpr Vec2<T> size()   const noexcept { return max - min; }
        [[nodiscard]] constexpr T        width() const noexcept { return max.x - min.x; }
        [[nodiscard]] constexpr T        height()const noexcept { return max.y - min.y; }
        [[nodiscard]] constexpr Vec2<T>  center()const noexcept { return (min + max) / T{ 2 }; }

        [[nodiscard]] constexpr bool isValid() const noexcept { return min.x <= max.x && min.y <= max.y; }
        [[nodiscard]] constexpr bool isEmpty() const noexcept { return width() <= T{} || height() <= T{}; }

        [[nodiscard]] constexpr bool contains(Vec2<T> p) const noexcept {
            return p.x >= min.x && p.x <= max.x &&
                p.y >= min.y && p.y <= max.y;
        }

        [[nodiscard]] constexpr bool intersects(const Rect& o) const noexcept {
            return !(o.min.x >= max.x || o.max.x <= min.x ||
                o.min.y >= max.y || o.max.y <= min.y);
        }

        [[nodiscard]] constexpr auto area() const noexcept {
            using CT = std::conditional_t<std::is_integral_v<T>, unsigned long long, long double>;
            return static_cast<CT>(width()) * static_cast<CT>(height());
        }

        /*-------------- mutating in-place --------------*/
        [[nodiscard]] constexpr Rect& translate(Vec2<T> off) noexcept {
            min += off; max += off; return *this;
        }

        template <typename F>
            requires std::is_floating_point_v<F>
        [[nodiscard]] constexpr Rect& scale(F factor, Vec2<T> pivot) noexcept {
            min = pivot + (min - pivot) * static_cast<T>(factor);
            max = pivot + (max - pivot) * static_cast<T>(factor);
            canonicalize();
            return *this;
        }

        template <typename F>
            requires std::is_floating_point_v<F>
        [[nodiscard]] constexpr Rect& scale(F factor) noexcept {
            return scale(factor, center());
        }

        [[nodiscard]] constexpr Rect& expand(T padding) noexcept {
            min -= Vec2<T>{padding, padding};
            max += Vec2<T>{padding, padding};
            canonicalize();
            return *this;
        }

        /*-------------- non-mutating helpers --------------*/
        [[nodiscard]] friend constexpr Rect translated(Rect r, Vec2<T> off) noexcept { return r.translate(off); }

        template <typename F> requires std::is_floating_point_v<F>
        [[nodiscard]] friend constexpr Rect scaled(Rect r, F f, Vec2<T> pivot) noexcept { return r.scale(f, pivot); }

        template <typename F> requires std::is_floating_point_v<F>
        [[nodiscard]] friend constexpr Rect scaled(Rect r, F f) noexcept { return r.scale(f); }

        [[nodiscard]] friend constexpr Rect
            intersection(const Rect& a, const Rect& b) noexcept {
            if (!a.intersects(b)) return Rect::empty();
            return { { std::max(a.min.x, b.min.x), std::max(a.min.y, b.min.y) },
                     { std::min(a.max.x, b.max.x), std::min(a.max.y, b.max.y) } };
        }

        [[nodiscard]] friend constexpr Rect
            unionRect(const Rect& a, const Rect& b) noexcept {
            return { { std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y) },
                     { std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y) } };
        }

        /*-------------- operatori --------------*/
        friend constexpr Rect operator+(Rect r, Vec2<T> off) noexcept { return r.translate(off); }
        friend constexpr Rect operator-(Rect r, Vec2<T> off) noexcept { return r.translate(-off); }

        friend constexpr bool operator==(const Rect&, const Rect&) = default;
        friend constexpr auto operator<=>(const Rect&, const Rect&) = default;

        friend std::ostream& operator<<(std::ostream& os, const Rect& r) {
            return os << "Rect(min=" << r.min << ", max=" << r.max << ")";
        }

        /*-------------- utilità statiche (solo FP) --------------*/
        template <typename U = T> requires std::is_floating_point_v<U>
        [[nodiscard]] static constexpr Rect<U> infinite() noexcept {
            constexpr U inf = std::numeric_limits<U>::infinity();
            return { Vec2<U>{-inf, -inf}, Vec2<U>{inf, inf} };
        }

        template <typename U = T> requires std::is_floating_point_v<U>
        [[nodiscard]] static constexpr Rect<U> nan() noexcept {
            const U n = std::numeric_limits<U>::quiet_NaN();
            return { Vec2<U>{n, n}, Vec2<U>{n, n} };
        }
    };

    /* alias comodi */
    using Rectf = Rect<float>;
    using Rectd = Rect<double>;
    using Recti = Rect<int>;

} // namespace math

/*-------------- hash specialisation --------------*/
namespace std {
    template <math::Arithmetic T>
    struct hash<math::Rect<T>> {
        size_t operator()(const math::Rect<T>& r) const noexcept {
            size_t seed = 0x9e3779b97f4a7c15ULL;
            auto mix = [&seed](size_t h) {
                seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
                };
            mix(std::hash<T>{}(r.min.x));
            mix(std::hash<T>{}(r.min.y));
            mix(std::hash<T>{}(r.max.x));
            mix(std::hash<T>{}(r.max.y));
            return seed;
        }
    };
} // namespace std

#pragma once
// math/rect.hpp – AABB 2-D generic

#include "Vec.h"

#include <algorithm>      // std::min, std::max, std::clamp
#include <cassert>
#include <cmath>          // std::nan
#include <limits>
#include <ostream>
#include <type_traits>

namespace math {

    template <Arithmetic T>
    struct Rect {
        Vec2<T> min{ };   // bottom-left
        Vec2<T> max{ };   // top-right

        /*-------------- costruttori / factory --------------*/
        constexpr Rect() noexcept = default;

        constexpr Rect(const Vec2<T>& mn, const Vec2<T>& mx) noexcept
            : min(mn), max(mx) {
            assert(isValid());
        }

        static constexpr Rect fromMinMax(const Vec2<T>& mn, const Vec2<T>& mx) {
            return Rect{ mn, mx };
        }
        static constexpr Rect fromPosSize(const Vec2<T>& pos, const Vec2<T>& size) {
            return Rect{ pos, pos + size };
        }
        static constexpr Rect fromCenterSize(const Vec2<T>& c, T w, T h) {
            Vec2<T> half{ w / T{2}, h / T{2} };
            return Rect{ c - half, c + half };
        }

        /*-------------- query --------------*/
        [[nodiscard]] constexpr Vec2<T> size()   const noexcept { return max - min; }
        [[nodiscard]] constexpr T        width() const noexcept { return max.x - min.x; }
        [[nodiscard]] constexpr T        height()const noexcept { return max.y - min.y; }
        [[nodiscard]] constexpr Vec2<T>  center()const noexcept { return (min + max) / T{ 2 }; }

        [[nodiscard]] constexpr auto area() const noexcept {
            using CT = std::common_type_t<T, double>;
            return static_cast<CT>(width()) * static_cast<CT>(height());
        }

        [[nodiscard]] constexpr bool isValid() const noexcept {
            return min.x <= max.x && min.y <= max.y;
        }

        [[nodiscard]] constexpr bool contains(const Vec2<T>& p) const noexcept {
            return p.x >= min.x && p.x <= max.x &&
                p.y >= min.y && p.y <= max.y;
        }

        [[nodiscard]] constexpr bool intersects(const Rect& o) const noexcept {
            return !(o.min.x >= max.x || o.max.x <= min.x ||
                o.min.y >= max.y || o.max.y <= min.y);
        }

        /*-------------- mutating in-place --------------*/
        constexpr Rect& translate(const Vec2<T>& off) noexcept {
            min += off; max += off; return *this;
        }

        constexpr Rect& scale(T factor, Vec2<T> pivot) noexcept {
            min = pivot + (min - pivot) * factor;
            max = pivot + (max - pivot) * factor;
            return *this;
        }

        constexpr Rect& scale(T factor) noexcept { return scale(factor, center()); }

        constexpr Rect& expand(T padding) noexcept {
            min -= Vec2<T>{padding, padding};
            max += Vec2<T>{padding, padding};
            return *this;
        }

        /*-------------- non-mutating helpers --------------*/
        [[nodiscard]] friend constexpr Rect translated(Rect r, const Vec2<T>& off) noexcept { return r.translate(off); }

        [[nodiscard]] friend constexpr Rect scaled(Rect r, T factor, Vec2<T> pivot) noexcept { return r.scale(factor, pivot); }

        [[nodiscard]] friend constexpr Rect scaled(Rect r, T factor) noexcept { return r.scale(factor); }

        [[nodiscard]] friend constexpr Rect
            intersection(const Rect& a, const Rect& b) noexcept {
            if (!a.intersects(b)) return {};   // rettangolo vuoto
            return { { std::max(a.min.x, b.min.x), std::max(a.min.y, b.min.y) },
                     { std::min(a.max.x, b.max.x), std::min(a.max.y, b.max.y) } };
        }

        [[nodiscard]] friend constexpr Rect
            unionRect(const Rect& a, const Rect& b) noexcept {
            return { { std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y) },
                     { std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y) } };
        }

        /*-------------- operatori --------------*/
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

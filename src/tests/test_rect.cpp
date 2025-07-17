// tests/test_rect.cpp
#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../math/Vec.h"
#include "../math/Rect.h"

using namespace math;

TEST_CASE("Rect constructors & canonicalization", "[rect][ctor]") {
    // min e max invertiti devono essere ri–ordinati
    constexpr Recti r1{ {5, 6}, {1, 2} };
    static_assert(r1.min == Vec2i{ 1, 2 });
    static_assert(r1.max == Vec2i{ 5, 6 });

    auto r2 = Rectf::fromPosSize({ 0.f, 0.f }, { 10.f, 20.f });
    REQUIRE(r2.max == Vec2f{ 10.f, 20.f });

    auto r3 = Rectd::fromCenterSize({ 0.0, 0.0 }, 4.0, 6.0);
    REQUIRE(r3.width() == Approx(4.0));
    REQUIRE(r3.height() == Approx(6.0));
    REQUIRE(r3.center() == Vec2d{ 0.0, 0.0 });
}

TEST_CASE("Queries: size, contains, intersects, area", "[rect][query]") {
    Recti r{ {0,0}, {4,3} };
    REQUIRE(r.size() == Vec2i{ 4,3 });
    REQUIRE(r.area() == 12);
    REQUIRE(r.contains({ 2,1 }));
    REQUIRE_FALSE(r.contains({ 4,3 }));        // bordo destro escluso
    Recti s{ {2,1},{5,4} };
    REQUIRE(r.intersects(s));
    Recti t{ {5,5},{6,6} };
    REQUIRE_FALSE(r.intersects(t));
}

TEST_CASE("Translate, scale, expand", "[rect][mutate]") {
    Rectf r{ {0,0}, {2,2} };
    r.translate({ 1,1 });
    REQUIRE(r.min == Vec2f{ 1,1 });
    REQUIRE(r.max == Vec2f{ 3,3 });

    r.scale(2.0f, Vec2f{ 2,2 });               // fattore 2 attorno al pivot (2,2)
    REQUIRE(r.min == Vec2f{ 0,0 });
    REQUIRE(r.max == Vec2f{ 4,4 });

    r.expand(1.f);
    REQUIRE(r.min == Vec2f{ -1,-1 });
    REQUIRE(r.max == Vec2f{ 5, 5 });
}

TEST_CASE("Intersection and union", "[rect][alg]") {
    const Recti a{ {0,0},{3,3} };
    const Recti b{ {2,2},{5,5} };
    auto i = intersection(a, b);
    REQUIRE(i == Recti{ {2,2},{3,3} });

    auto u = unionRect(a, b);
    REQUIRE(u == Recti{ {0,0},{5,5} });
}

TEST_CASE("Empty, infinite & NaN", "[rect][edge]") {
    Rectd empty = Rectd::empty();
    REQUIRE(empty.isEmpty());
    REQUIRE_FALSE(empty.isValid());          // min == max == (0,0) ➜ non valido come area

    auto inf = Rectd::infinite();
    REQUIRE(std::isinf(inf.max.x));
    REQUIRE_FALSE(inf.isEmpty());
}

TEST_CASE("Operators and comparison", "[rect][ops]") {
    constexpr Recti r1{ {0,0},{1,1} };
    constexpr Recti r2 = r1 + Vec2i{ 2,3 };
    static_assert(r2.min == Vec2i{ 2,3 });
    static_assert(r2 == Recti{ {2,3},{3,4} });

    static_assert((r1 <=> r2) == std::strong_ordering::less);
}

// tests/test_vec.cpp
#include <catch2/catch_all.hpp>
#include "../math/Vec.h"

using namespace math;

TEST_CASE("Vec basic construction", "[vec]") {
    constexpr Vec2i vi{ 1, 2 };
    static_assert(vi[0] == 1 && vi[1] == 2);

    Vec<3, double> vd{ 1.0, 2.0, 3.0 };
    REQUIRE(vd[2] == 3.0);
}

TEST_CASE("Arithmetic operators", "[vec][ops]") {
    Vec2f a{ 1.f, 2.f };
    Vec2f b{ 3.f, 4.f };

    SECTION("add / subtract") {
        REQUIRE((a + b) == Vec2f{ 4.f, 6.f });
        REQUIRE((b - a) == Vec2f{ 2.f, 2.f });
    }
    SECTION("scalar multiply / divide") {
        REQUIRE((a * 2.f) == Vec2f{ 2.f, 4.f });
        REQUIRE((6.f * b) == Vec2f{ 18.f, 24.f });
        REQUIRE((b / 2.f) == Vec2f{ 1.5f, 2.f });
        REQUIRE_THROWS_AS(b / 0.f, std::domain_error);
    }
}

TEST_CASE("Dot, length, normalize", "[vec][math]") {
    Vec3d v{ 3.0, 4.0, 0.0 };
    REQUIRE(v.dot(v) == Approx(25.0));
    REQUIRE(v.lengthSquared() == 25.0);
    REQUIRE(v.length() == Approx(5.0));
    auto n = v.normalized();
    REQUIRE(n.length() == Approx(1.0));
}

TEST_CASE("Cross product", "[vec][cross]") {
    Vec2i u2{ 1, 0 }, v2{ 0, 1 };
    REQUIRE(cross(u2, v2) == 1);

    Vec3i x{ 1,0,0 }, y{ 0,1,0 };
    REQUIRE(cross(x, y) == Vec3i{ 0,0,1 });
}

TEST_CASE("Clamp", "[vec][clamp]") {
    Vec3f v{ -1.f, 0.5f,  2.f };
    Vec3f lo{ 0.f, 0.f, 0.f };
    Vec3f hi{ 1.f, 1.f, 1.f };
    REQUIRE(clamp(v, lo, hi) == Vec3f{ 0.f, 0.5f, 1.f });
}

TEST_CASE("Comparison operators", "[vec][compare]") {
    constexpr Vec4i a{ 1,2,3,4 };
    constexpr Vec4i b{ 1,2,3,4 };
    constexpr Vec4i c{ 4,3,2,1 };
    static_assert(a == b);
    static_assert(a != c);
    REQUIRE(a <=> c == std::strong_ordering::less);
}

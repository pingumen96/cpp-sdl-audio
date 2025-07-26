#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../math/math.h"

using namespace math;
using Catch::Approx;

TEST_CASE("Math Library - Vec3 Operations", "[math][vec3]") {
    SECTION("Construction and component access") {
        Vec3f v1(1.0f, 2.0f, 3.0f);
        Vec3f v2(2.0f);  // Single value constructor
        
        REQUIRE(v1.x() == Approx(1.0f));
        REQUIRE(v1.y() == Approx(2.0f));
        REQUIRE(v1.z() == Approx(3.0f));
        
        REQUIRE(v2.x() == Approx(2.0f));
        REQUIRE(v2.y() == Approx(2.0f));
        REQUIRE(v2.z() == Approx(2.0f));
    }
    
    SECTION("Arithmetic operations") {
        Vec3f a(1.0f, 2.0f, 3.0f);
        Vec3f b(4.0f, 5.0f, 6.0f);
        
        Vec3f sum = a + b;
        REQUIRE(sum.x() == Approx(5.0f));
        REQUIRE(sum.y() == Approx(7.0f));
        REQUIRE(sum.z() == Approx(9.0f));
        
        Vec3f diff = b - a;
        REQUIRE(diff.x() == Approx(3.0f));
        REQUIRE(diff.y() == Approx(3.0f));
        REQUIRE(diff.z() == Approx(3.0f));
        
        Vec3f scaled = a * 2.0f;
        REQUIRE(scaled.x() == Approx(2.0f));
        REQUIRE(scaled.y() == Approx(4.0f));
        REQUIRE(scaled.z() == Approx(6.0f));
    }
    
    SECTION("Compound assignment operations") {
        Vec3f v(1.0f, 2.0f, 3.0f);
        
        v += Vec3f(1.0f, 1.0f, 1.0f);
        REQUIRE(v.x() == Approx(2.0f));
        REQUIRE(v.y() == Approx(3.0f));
        REQUIRE(v.z() == Approx(4.0f));
        
        v *= 0.5f;
        REQUIRE(v.x() == Approx(1.0f));
        REQUIRE(v.y() == Approx(1.5f));
        REQUIRE(v.z() == Approx(2.0f));
    }
    
    SECTION("Dot product and length") {
        Vec3f a(1.0f, 0.0f, 0.0f);
        Vec3f b(0.0f, 1.0f, 0.0f);
        Vec3f c(1.0f, 1.0f, 0.0f);
        
        REQUIRE(dot(a, b) == Approx(0.0f));
        REQUIRE(dot(a, a) == Approx(1.0f));
        REQUIRE(dot(c, c) == Approx(2.0f));
        
        REQUIRE(a.length() == Approx(1.0f));
        REQUIRE(a.lengthSquared() == Approx(1.0f));
        REQUIRE(c.length() == Approx(std::sqrt(2.0f)));
    }
    
    SECTION("Cross product") {
        Vec3f x(1.0f, 0.0f, 0.0f);
        Vec3f y(0.0f, 1.0f, 0.0f);
        Vec3f z(0.0f, 0.0f, 1.0f);
        
        Vec3f cross_xy = cross(x, y);
        REQUIRE(cross_xy.x() == Approx(0.0f));
        REQUIRE(cross_xy.y() == Approx(0.0f));
        REQUIRE(cross_xy.z() == Approx(1.0f));
        
        Vec3f cross_yz = cross(y, z);
        REQUIRE(cross_yz.x() == Approx(1.0f));
        REQUIRE(cross_yz.y() == Approx(0.0f));
        REQUIRE(cross_yz.z() == Approx(0.0f));
    }
}

TEST_CASE("Math Library - Matrix4 Operations", "[math][matrix4]") {
    SECTION("Identity matrix") {
        Matrix4f identity = Matrix4f::identity();
        
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i == j) {
                    REQUIRE(identity(i, j) == Approx(1.0f));
                } else {
                    REQUIRE(identity(i, j) == Approx(0.0f));
                }
            }
        }
    }
    
    SECTION("Translation matrix") {
        Vec3f translation(1.0f, 2.0f, 3.0f);
        Matrix4f identity = Matrix4f::identity();
        Matrix4f trans = math::translate(identity, translation);
        
        REQUIRE(trans(0, 3) == Approx(1.0f));
        REQUIRE(trans(1, 3) == Approx(2.0f));
        REQUIRE(trans(2, 3) == Approx(3.0f));
        REQUIRE(trans(3, 3) == Approx(1.0f));
        
        // Diagonal should be 1
        REQUIRE(trans(0, 0) == Approx(1.0f));
        REQUIRE(trans(1, 1) == Approx(1.0f));
        REQUIRE(trans(2, 2) == Approx(1.0f));
    }
    
    SECTION("Scale matrix") {
        Vec3f scale(2.0f, 3.0f, 4.0f);
        Matrix4f scaleMatrix = Matrix4f::scale(scale);
        
        REQUIRE(scaleMatrix(0, 0) == Approx(2.0f));
        REQUIRE(scaleMatrix(1, 1) == Approx(3.0f));
        REQUIRE(scaleMatrix(2, 2) == Approx(4.0f));
        REQUIRE(scaleMatrix(3, 3) == Approx(1.0f));
        
        // Non-diagonal should be 0
        REQUIRE(scaleMatrix(0, 1) == Approx(0.0f));
        REQUIRE(scaleMatrix(1, 0) == Approx(0.0f));
    }
    
    SECTION("Matrix equality") {
        Matrix4f a = Matrix4f::identity();
        Matrix4f b = Matrix4f::identity();
        Matrix4f c = Matrix4f::scale(Vec3f(2.0f));
        
        REQUIRE(a == b);
        REQUIRE_FALSE(a == c);
    }
}

TEST_CASE("Math Library - Quaternion Operations", "[math][quaternion]") {
    SECTION("Identity quaternion") {
        Quatf identity;
        
        REQUIRE(identity.w == Approx(1.0f));
        REQUIRE(identity.x == Approx(0.0f));
        REQUIRE(identity.y == Approx(0.0f));
        REQUIRE(identity.z == Approx(0.0f));
    }
    
    SECTION("Axis-angle construction") {
        Vec3f axis(0.0f, 1.0f, 0.0f);  // Y-axis
        float angle = M_PI / 2.0f;      // 90 degrees
        
        Quatf rotation(angle, axis);
        
        // For 90 degree rotation around Y-axis:
        // w = cos(45°) = √2/2, y = sin(45°) = √2/2
        REQUIRE(rotation.w == Approx(std::cos(angle / 2.0f)));
        REQUIRE(rotation.x == Approx(0.0f));
        REQUIRE(rotation.y == Approx(std::sin(angle / 2.0f)));
        REQUIRE(rotation.z == Approx(0.0f));
    }
    
    SECTION("Quaternion normalization") {
        Quatf q(2.0f, 1.0f, 2.0f, 3.0f);
        float originalLength = std::sqrt(2*2 + 1*1 + 2*2 + 3*3);
        
        Quatf normalized = normalize(q);
        float newLength = std::sqrt(normalized.w*normalized.w + 
                                   normalized.x*normalized.x + 
                                   normalized.y*normalized.y + 
                                   normalized.z*normalized.z);
        
        REQUIRE(newLength == Approx(1.0f));
        REQUIRE(normalized.w == Approx(2.0f / originalLength));
    }
}

TEST_CASE("Math Library - Utility Functions", "[math][utils]") {
    SECTION("Angle conversions") {
        float degrees = 180.0f;
        float rad = math::radians(degrees);
        float backToDegrees = math::degrees(rad);
        
        REQUIRE(rad == Approx(M_PI));
        REQUIRE(backToDegrees == Approx(degrees));
    }
    
    SECTION("Trigonometric functions") {
        REQUIRE(std::sin(math::radians(0.0f)) == Approx(0.0f));
        REQUIRE(std::sin(math::radians(90.0f)) == Approx(1.0f));
        REQUIRE(std::cos(math::radians(0.0f)) == Approx(1.0f));
        REQUIRE(std::cos(math::radians(90.0f)) == Approx(0.0f).margin(1e-6));
    }
}

TEST_CASE("Math Library - GLM Compatibility Functions", "[math][glm-compat]") {
    SECTION("lookAt function") {
        Vec3f eye(0.0f, 0.0f, 5.0f);
        Vec3f center(0.0f, 0.0f, 0.0f);
        Vec3f up(0.0f, 1.0f, 0.0f);
        
        Matrix4f view = math::lookAt(eye, center, up);
        
        // The result should be a valid view matrix
        // For this simple case, translation should be (0, 0, -5)
        REQUIRE(view(0, 3) == Approx(0.0f));
        REQUIRE(view(1, 3) == Approx(0.0f));
        REQUIRE(view(2, 3) == Approx(-5.0f));
    }
    
    SECTION("perspective function") {
        float fov = math::radians(45.0f);
        float aspect = 16.0f / 9.0f;
        float near = 0.1f;
        float far = 100.0f;
        
        Matrix4f proj = math::perspective(fov, aspect, near, far);
        
        // The result should be a valid perspective matrix
        // Check that it's not identity and has proper structure
        REQUIRE_FALSE(proj == Matrix4f::identity());
        REQUIRE(proj(2, 3) == Approx(-1.0f));  // W coordinate transformation
        
        // Check Z transformation coefficient (correct for perspective projection)
        float expected_z = -(far + near) / (far - near);
        REQUIRE(proj(2, 2) == Approx(expected_z));
        
        // Check perspective divide coefficient
        float expected_w = -(2.0f * far * near) / (far - near);
        REQUIRE(proj(3, 2) == Approx(expected_w));
    }
}

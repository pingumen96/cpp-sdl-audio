#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../math/Matrix.h"
#include "../math/Vec.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace math;
using Catch::Approx;

// Helper function for comparing matrices with floating point tolerance
bool matrixApproxEqual(const Mat4& a, const Mat4& b, float tolerance = 0.001f) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (std::abs(a(i, j) - b(i, j)) > tolerance) {
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Matrix4 Construction", "[Matrix4]") {
    SECTION("Default constructor creates identity matrix") {
        Mat4 identity;

        // Check diagonal elements are 1
        REQUIRE(identity(0, 0) == Approx(1.0f));
        REQUIRE(identity(1, 1) == Approx(1.0f));
        REQUIRE(identity(2, 2) == Approx(1.0f));
        REQUIRE(identity(3, 3) == Approx(1.0f));

        // Check off-diagonal elements are 0
        REQUIRE(identity(0, 1) == Approx(0.0f));
        REQUIRE(identity(0, 2) == Approx(0.0f));
        REQUIRE(identity(1, 0) == Approx(0.0f));
        REQUIRE(identity(2, 0) == Approx(0.0f));
    }

    SECTION("Diagonal constructor") {
        Mat4 scaled(2.5f);

        REQUIRE(scaled(0, 0) == Approx(2.5f));
        REQUIRE(scaled(1, 1) == Approx(2.5f));
        REQUIRE(scaled(2, 2) == Approx(2.5f));
        REQUIRE(scaled(3, 3) == Approx(2.5f));

        // Off-diagonal should be zero
        REQUIRE(scaled(0, 1) == Approx(0.0f));
        REQUIRE(scaled(1, 0) == Approx(0.0f));
    }

    SECTION("Copy constructor") {
        Mat4 original;
        original(0, 3) = 5.0f;
        original(2, 1) = 3.0f;

        Mat4 copy(original);

        REQUIRE(copy(0, 3) == Approx(5.0f));
        REQUIRE(copy(2, 1) == Approx(3.0f));
        REQUIRE(copy(1, 1) == Approx(1.0f)); // Identity diagonal
    }
}

TEST_CASE("Matrix4 Element Access", "[Matrix4]") {
    SECTION("Operator() access") {
        Mat4 mat;

        // Test writing
        mat(1, 2) = 7.5f;
        mat(3, 0) = -2.0f;

        // Test reading
        REQUIRE(mat(1, 2) == Approx(7.5f));
        REQUIRE(mat(3, 0) == Approx(-2.0f));
    }

    SECTION("Operator[] column access") {
        Mat4 mat;

        // Modify entire column
        mat[1][0] = 1.0f;
        mat[1][1] = 2.0f;
        mat[1][2] = 3.0f;
        mat[1][3] = 4.0f;

        REQUIRE(mat(0, 1) == Approx(1.0f));
        REQUIRE(mat(1, 1) == Approx(2.0f));
        REQUIRE(mat(2, 1) == Approx(3.0f));
        REQUIRE(mat(3, 1) == Approx(4.0f));
    }

    SECTION("Data pointer for OpenGL") {
        Mat4 mat;
        mat(0, 0) = 1.0f;
        mat(1, 0) = 2.0f;
        mat(0, 1) = 3.0f;

        const float* data = mat.data();

        // Column-major layout: first column comes first
        REQUIRE(data[0] == Approx(1.0f));  // (0,0)
        REQUIRE(data[1] == Approx(2.0f));  // (1,0)
        REQUIRE(data[4] == Approx(3.0f));  // (0,1)
    }
}

TEST_CASE("Matrix4 Arithmetic Operations", "[Matrix4]") {
    SECTION("Matrix addition") {
        Mat4 a;
        a(0, 0) = 1.0f; a(0, 1) = 2.0f;
        a(1, 0) = 3.0f; a(1, 1) = 4.0f;

        Mat4 b;
        b(0, 0) = 5.0f; b(0, 1) = 6.0f;
        b(1, 0) = 7.0f; b(1, 1) = 8.0f;

        Mat4 c = a + b;

        REQUIRE(c(0, 0) == Approx(6.0f));
        REQUIRE(c(0, 1) == Approx(8.0f));
        REQUIRE(c(1, 0) == Approx(10.0f));
        REQUIRE(c(1, 1) == Approx(12.0f));
    }

    SECTION("Matrix subtraction") {
        Mat4 a;
        a(0, 0) = 10.0f; a(0, 1) = 8.0f;

        Mat4 b;
        b(0, 0) = 3.0f; b(0, 1) = 2.0f;

        Mat4 c = a - b;

        REQUIRE(c(0, 0) == Approx(7.0f));
        REQUIRE(c(0, 1) == Approx(6.0f));
    }

    SECTION("Matrix multiplication") {
        Mat4 a;
        a(0, 0) = 1.0f; a(0, 1) = 2.0f; a(0, 2) = 0.0f; a(0, 3) = 0.0f;
        a(1, 0) = 0.0f; a(1, 1) = 1.0f; a(1, 2) = 3.0f; a(1, 3) = 0.0f;
        a(2, 0) = 0.0f; a(2, 1) = 0.0f; a(2, 2) = 1.0f; a(2, 3) = 0.0f;
        a(3, 0) = 0.0f; a(3, 1) = 0.0f; a(3, 2) = 0.0f; a(3, 3) = 1.0f;

        Mat4 b;
        b(0, 0) = 2.0f; b(0, 1) = 0.0f; b(0, 2) = 0.0f; b(0, 3) = 0.0f;
        b(1, 0) = 0.0f; b(1, 1) = 2.0f; b(1, 2) = 0.0f; b(1, 3) = 0.0f;
        b(2, 0) = 0.0f; b(2, 1) = 0.0f; b(2, 2) = 2.0f; b(2, 3) = 0.0f;
        b(3, 0) = 0.0f; b(3, 1) = 0.0f; b(3, 2) = 0.0f; b(3, 3) = 1.0f;

        Mat4 c = a * b;

        REQUIRE(c(0, 0) == Approx(2.0f));
        REQUIRE(c(0, 1) == Approx(4.0f));
        REQUIRE(c(1, 2) == Approx(6.0f));
    }

    SECTION("Scalar multiplication") {
        Mat4 a;
        a(0, 0) = 2.0f;
        a(1, 1) = 3.0f;

        Mat4 b = a * 2.5f;

        REQUIRE(b(0, 0) == Approx(5.0f));
        REQUIRE(b(1, 1) == Approx(7.5f));
    }

    SECTION("Assignment operators") {
        Mat4 a;
        a(0, 0) = 1.0f;

        Mat4 b;
        b(0, 0) = 2.0f;

        a += b;
        REQUIRE(a(0, 0) == Approx(3.0f));

        a *= 2.0f;
        REQUIRE(a(0, 0) == Approx(6.0f));
    }
}

TEST_CASE("Matrix4 Vector Transformation", "[Matrix4]") {
    SECTION("Vector multiplication") {
        Mat4 scale = Mat4::scale(2.0f, 3.0f, 4.0f);
        Vec4f vec(1.0f, 1.0f, 1.0f, 1.0f);

        Vec4f result = scale * vec;

        REQUIRE(result[0] == Approx(2.0f));
        REQUIRE(result[1] == Approx(3.0f));
        REQUIRE(result[2] == Approx(4.0f));
        REQUIRE(result[3] == Approx(1.0f));
    }

    SECTION("Translation transformation") {
        Mat4 translation = Mat4::translation(5.0f, -2.0f, 3.0f);
        Vec4f point(1.0f, 1.0f, 1.0f, 1.0f);

        Vec4f result = translation * point;

        REQUIRE(result[0] == Approx(6.0f));
        REQUIRE(result[1] == Approx(-1.0f));
        REQUIRE(result[2] == Approx(4.0f));
        REQUIRE(result[3] == Approx(1.0f));
    }
}

TEST_CASE("Matrix4 Transformation Factories", "[Matrix4]") {
    SECTION("Identity matrix") {
        Mat4 identity = Mat4::identity();
        Mat4 default_constructed;

        REQUIRE(matrixApproxEqual(identity, default_constructed));
    }

    SECTION("Translation matrix") {
        Mat4 trans = Mat4::translation(1.0f, 2.0f, 3.0f);

        REQUIRE(trans(0, 3) == Approx(1.0f));
        REQUIRE(trans(1, 3) == Approx(2.0f));
        REQUIRE(trans(2, 3) == Approx(3.0f));
        REQUIRE(trans(3, 3) == Approx(1.0f));

        // Should preserve identity for rotation part
        REQUIRE(trans(0, 0) == Approx(1.0f));
        REQUIRE(trans(1, 1) == Approx(1.0f));
        REQUIRE(trans(2, 2) == Approx(1.0f));
    }

    SECTION("Scale matrix") {
        Mat4 scale = Mat4::scale(2.0f, 3.0f, 4.0f);

        REQUIRE(scale(0, 0) == Approx(2.0f));
        REQUIRE(scale(1, 1) == Approx(3.0f));
        REQUIRE(scale(2, 2) == Approx(4.0f));
        REQUIRE(scale(3, 3) == Approx(1.0f));

        // Off-diagonal should be zero
        REQUIRE(scale(0, 1) == Approx(0.0f));
        REQUIRE(scale(1, 0) == Approx(0.0f));
    }

    SECTION("Uniform scale matrix") {
        Mat4 scale = Mat4::scale(2.5f);

        REQUIRE(scale(0, 0) == Approx(2.5f));
        REQUIRE(scale(1, 1) == Approx(2.5f));
        REQUIRE(scale(2, 2) == Approx(2.5f));
        REQUIRE(scale(3, 3) == Approx(1.0f));
    }

    SECTION("Rotation matrices") {
        float angle = static_cast<float>(M_PI / 4); // 45 degrees
        float cos45 = std::cos(angle);
        float sin45 = std::sin(angle);

        SECTION("X rotation") {
            Mat4 rotX = Mat4::rotationX(angle);

            REQUIRE(rotX(0, 0) == Approx(1.0f));
            REQUIRE(rotX(1, 1) == Approx(cos45));
            REQUIRE(rotX(1, 2) == Approx(sin45));
            REQUIRE(rotX(2, 1) == Approx(-sin45));
            REQUIRE(rotX(2, 2) == Approx(cos45));
        }

        SECTION("Y rotation") {
            Mat4 rotY = Mat4::rotationY(angle);

            REQUIRE(rotY(0, 0) == Approx(cos45));
            REQUIRE(rotY(0, 2) == Approx(-sin45));
            REQUIRE(rotY(1, 1) == Approx(1.0f));
            REQUIRE(rotY(2, 0) == Approx(sin45));
            REQUIRE(rotY(2, 2) == Approx(cos45));
        }

        SECTION("Z rotation") {
            Mat4 rotZ = Mat4::rotationZ(angle);

            REQUIRE(rotZ(0, 0) == Approx(cos45));
            REQUIRE(rotZ(0, 1) == Approx(-sin45));
            REQUIRE(rotZ(1, 0) == Approx(sin45));
            REQUIRE(rotZ(1, 1) == Approx(cos45));
            REQUIRE(rotZ(2, 2) == Approx(1.0f));
        }
    }
}

TEST_CASE("Matrix4 Projection Matrices", "[Matrix4]") {
    SECTION("Orthographic projection") {
        Mat4 ortho = Mat4::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

        // Test that the matrix maps the viewing volume correctly
        Vec4f center(0.0f, 0.0f, -50.05f, 1.0f); // Middle of near/far
        Vec4f projected = ortho * center;

        // Z should be mapped to middle of [-1, 1] range
        REQUIRE(projected[2] == Approx(0.0f).margin(0.01f));
    }

    SECTION("Perspective projection") {
        float fovy = static_cast<float>(M_PI / 4); // 45 degrees
        float aspect = 16.0f / 9.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        Mat4 persp = Mat4::perspective(fovy, aspect, nearPlane, farPlane);

        // The perspective matrix should have specific structure
        REQUIRE(persp(2, 3) == Approx(-1.0f));
        REQUIRE(persp(3, 3) == Approx(0.0f));

        // Aspect ratio should be reflected in the matrix
        float expectedX = 1.0f / (aspect * std::tan(fovy / 2.0f));
        REQUIRE(persp(0, 0) == Approx(expectedX));
    }
}

TEST_CASE("Matrix4 Utility Functions", "[Matrix4]") {
    SECTION("Transpose") {
        Mat4 mat;
        mat(0, 1) = 5.0f;
        mat(1, 0) = 3.0f;
        mat(2, 3) = 7.0f;

        Mat4 transposed = mat.transpose();

        REQUIRE(transposed(1, 0) == Approx(5.0f));
        REQUIRE(transposed(0, 1) == Approx(3.0f));
        REQUIRE(transposed(3, 2) == Approx(7.0f));
    }

    SECTION("Comparison operators") {
        Mat4 a;
        Mat4 b;

        REQUIRE(a == b);

        b(1, 1) = 2.0f;
        REQUIRE(a != b);
    }
}

TEST_CASE("Matrix4 Complex Transformations", "[Matrix4]") {
    SECTION("Combined transformations") {
        // Create a transformation pipeline: Scale -> Rotate -> Translate
        Mat4 scale = Mat4::scale(2.0f);
        Mat4 rotation = Mat4::rotationZ(static_cast<float>(M_PI / 2)); // 90 degrees
        Mat4 translation = Mat4::translation(1.0f, 0.0f, 0.0f);

        Mat4 combined = translation * rotation * scale;

        // Test a point transformation
        Vec4f point(1.0f, 0.0f, 0.0f, 1.0f);
        Vec4f result = combined * point;

        // After scale: (2, 0, 0)
        // After rotation: (0, 2, 0)
        // After translation: (1, 2, 0)
        REQUIRE(result[0] == Approx(1.0f));
        REQUIRE(result[1] == Approx(2.0f));
        REQUIRE(result[2] == Approx(0.0f));
    }
}

TEST_CASE("Matrix4 Edge Cases", "[Matrix4]") {
    SECTION("Zero matrix") {
        Mat4 zero(0.0f);

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                REQUIRE(zero(i, j) == Approx(0.0f));
            }
        }
    }

    SECTION("Large values") {
        Mat4 large(1000000.0f);

        REQUIRE(large(0, 0) == Approx(1000000.0f));
        REQUIRE(large(3, 3) == Approx(1000000.0f));
    }

    SECTION("Negative values") {
        Mat4 negative(-5.0f);

        REQUIRE(negative(1, 1) == Approx(-5.0f));
        REQUIRE(negative(0, 1) == Approx(0.0f));
    }
}
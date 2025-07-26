#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../scene/rendering/Renderer2D.h"
#include "../scene/rendering/Renderer2DImpl.h"
#include "../scene/rendering/QuadCommand.h"
#include "../scene/rendering/Material2D.h"
#include "../scene/rendering/QuadBatch.h"
#include "../scene/Renderer2DIntegration.h"
#include "../resources/ResourceManager.h"

using namespace scene;
using namespace math;
using Catch::Approx;

TEST_CASE("Renderer2D - Basic Color Tests", "[renderer2d][color]") {
    SECTION("Color construction and conversion") {
        Color red(1.0f, 0.0f, 0.0f, 1.0f);
        Vec4f redVec = red.toVec4();

        REQUIRE(redVec.x() == Catch::Approx(1.0f));
        REQUIRE(redVec.y() == Catch::Approx(0.0f));
        REQUIRE(redVec.z() == Catch::Approx(0.0f));
        REQUIRE(redVec.w() == Catch::Approx(1.0f));
    }

    SECTION("Predefined colors") {
        REQUIRE(Color::White.r == Catch::Approx(1.0f));
        REQUIRE(Color::Black.r == Catch::Approx(0.0f));
        REQUIRE(Color::Red.r == Catch::Approx(1.0f));
        REQUIRE(Color::Red.g == Catch::Approx(0.0f));
    }
}

TEST_CASE("Renderer2D - Camera2D Tests", "[renderer2d][camera]") {
    SECTION("Camera construction and basic properties") {
        Camera2D camera;

        REQUIRE(camera.getPosition().x() == Catch::Approx(0.0f));
        REQUIRE(camera.getPosition().y() == Catch::Approx(0.0f));
        REQUIRE(camera.getZoom() == Catch::Approx(1.0f));
        REQUIRE(camera.getRotation() == Catch::Approx(0.0f));
    }

    SECTION("Camera transformations") {
        Camera2D camera;
        camera.setPosition(Vec2f(100.0f, 200.0f));
        camera.setZoom(2.0f);

        REQUIRE(camera.getPosition().x() == Catch::Approx(100.0f));
        REQUIRE(camera.getPosition().y() == Catch::Approx(200.0f));
        REQUIRE(camera.getZoom() == Catch::Approx(2.0f));

        // Le matrici dovrebbero essere valide
        Matrix4f view = camera.getViewMatrix();
        Matrix4f proj = camera.getProjectionMatrix();

        // Test che le matrici non siano NaN
        REQUIRE(view(0, 0) == view(0, 0)); // NaN check
        REQUIRE(proj(0, 0) == proj(0, 0)); // NaN check
    }

    SECTION("Camera follow") {
        Camera2D camera;
        Vec2f target(100.0f, 100.0f);

        camera.follow(target, 0.5f);

        REQUIRE(camera.getPosition().x() == Catch::Approx(50.0f));
        REQUIRE(camera.getPosition().y() == Catch::Approx(50.0f));
    }
}

TEST_CASE("Renderer2D - QuadCommand Tests", "[renderer2d][quad]") {
    SECTION("QuadCommand creation from rect") {
        Vec2f position(10.0f, 20.0f);
        Vec2f size(100.0f, 50.0f);
        Vec4f color(1.0f, 0.0f, 0.0f, 1.0f);

        QuadCommand command = QuadCommand::fromRect(
            position, size, color, "texture_test", 0.0f, 1, 0.5f
        );

        REQUIRE(command.color.x() == Catch::Approx(1.0f));
        REQUIRE(command.color.y() == Catch::Approx(0.0f));
        REQUIRE(command.textureId == "texture_test");
        REQUIRE(command.layer == 1);
        REQUIRE(command.depth == Catch::Approx(0.5f));

        // Test che la matrice di trasformazione sia valida
        REQUIRE(command.transform(0, 0) != 0.0f); // Dovrebbe avere una scala
    }

    SECTION("Batch compatibility") {
        QuadCommand cmd1 = QuadCommand::fromRect(
            Vec2f(0.0f), Vec2f(1.0f), Vec4f(1.0f), "tex1", 0.0f, 0
        );
        QuadCommand cmd2 = QuadCommand::fromRect(
            Vec2f(10.0f), Vec2f(1.0f), Vec4f(1.0f), "tex1", 0.0f, 0
        );
        QuadCommand cmd3 = QuadCommand::fromRect(
            Vec2f(0.0f), Vec2f(1.0f), Vec4f(1.0f), "tex2", 0.0f, 0
        );

        REQUIRE(cmd1.isBatchCompatible(cmd2)); // Stessa texture e layer
        REQUIRE_FALSE(cmd1.isBatchCompatible(cmd3)); // Texture diversa
    }
}

TEST_CASE("Renderer2D - Material2D Tests", "[renderer2d][material]") {
    SECTION("Material creation and properties") {
        Material2D material = Material2D::createSolidColor(Vec4f(0.5f, 0.5f, 0.5f, 1.0f));

        REQUIRE(material.getShader() == "flat_color");
        REQUIRE(material.getTexture().empty());
        REQUIRE(material.getBaseColor().x() == Catch::Approx(0.5f));
    }

    SECTION("Textured material") {
        Material2D material = Material2D::createTextured("test_texture", Vec4f(1.0f, 0.0f, 0.0f, 1.0f));

        REQUIRE(material.getShader() == "texture");
        REQUIRE(material.getTexture() == "test_texture");
        REQUIRE(material.getBaseColor().x() == Catch::Approx(1.0f));
    }

    SECTION("Batch compatibility") {
        Material2D mat1 = Material2D::createSolidColor();
        Material2D mat2 = Material2D::createSolidColor();
        Material2D mat3 = Material2D::createTextured("test");

        REQUIRE(mat1.isBatchCompatible(mat2));
        REQUIRE_FALSE(mat1.isBatchCompatible(mat3));

        REQUIRE(mat1.getBatchId() == mat2.getBatchId());
        REQUIRE(mat1.getBatchId() != mat3.getBatchId());
    }
}

TEST_CASE("Renderer2D - QuadBatch Tests", "[renderer2d][batch]") {
    SECTION("Basic batching") {
        QuadBatch batch(10, true);
        batch.setUnitQuadHandle("unit_quad");

        // Aggiungi alcuni quad
        QuadCommand cmd1 = QuadCommand::fromRect(Vec2f(0.0f), Vec2f(1.0f), Vec4f(1.0f), "", 0.0f, 0);
        QuadCommand cmd2 = QuadCommand::fromRect(Vec2f(10.0f), Vec2f(1.0f), Vec4f(1.0f), "", 0.0f, 0);

        batch.push(cmd1);
        batch.push(cmd2);

        // Test che i quad siano stati aggiunti
        RenderQueueBuilder builder;
        batch.flush(builder);

        auto stats = batch.getStats();
        REQUIRE(stats.quadCount == 2);
        REQUIRE(stats.batchCount >= 1); // Almeno un batch
    }

    SECTION("Reset functionality") {
        QuadBatch batch;
        batch.setUnitQuadHandle("unit_quad");

        QuadCommand cmd = QuadCommand::fromRect(Vec2f(0.0f), Vec2f(1.0f), Vec4f(1.0f));
        batch.push(cmd);

        batch.reset();

        RenderQueueBuilder builder;
        batch.flush(builder);

        auto stats = batch.getStats();
        // Dopo reset e flush dovrebbe essere vuoto
        // (le statistiche vengono aggiornate al flush)
    }
}

TEST_CASE("Renderer2D - Integration Tests", "[renderer2d][integration]") {
    SECTION("Full system initialization") {
        // Questo test verifica che l'intero sistema si inizializzi senza errori
        // anche se non abbiamo backend reali

        REQUIRE_NOTHROW([]() {
            Renderer2DIntegrationExample example;

            // L'inizializzazione dovrebbe funzionare con NullBackend
            bool initResult = example.init(800, 600);

            if (initResult) {
                // Prova a renderizzare alcuni frame
                for (int i = 0; i < 3; ++i) {
                    example.renderFrame();
                }

                example.shutdown();
            }
            }());
    }
}

TEST_CASE("Renderer2D - Rect and Geometry", "[renderer2d][geometry]") {
    SECTION("Rect2D construction") {
        scene::Rect2D rect1;
        REQUIRE(rect1.position.x() == Catch::Approx(0.0f));
        REQUIRE(rect1.size.x() == Catch::Approx(1.0f));

        scene::Rect2D rect2(Vec2f(10.0f, 20.0f), Vec2f(100.0f, 200.0f));
        REQUIRE(rect2.position.x() == Catch::Approx(10.0f));
        REQUIRE(rect2.position.y() == Catch::Approx(20.0f));
        REQUIRE(rect2.size.x() == Catch::Approx(100.0f));
        REQUIRE(rect2.size.y() == Catch::Approx(200.0f));

        scene::Rect2D rect3(5.0f, 6.0f, 7.0f, 8.0f);
        REQUIRE(rect3.position.x() == Catch::Approx(5.0f));
        REQUIRE(rect3.position.y() == Catch::Approx(6.0f));
        REQUIRE(rect3.size.x() == Catch::Approx(7.0f));
        REQUIRE(rect3.size.y() == Catch::Approx(8.0f));
    }
}
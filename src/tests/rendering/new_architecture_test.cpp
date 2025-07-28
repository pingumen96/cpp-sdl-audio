#include <catch2/catch_test_macros.hpp>
#include "../../rendering/hal/null/NullDevice.h"
#include "../../rendering/backend/RenderBackend.h"
#include "../../rendering/2d/Renderer2D.h"
#include "../../rendering/Rendering.h"

using namespace rendering;

TEST_CASE("New Rendering Architecture - HAL Layer", "[rendering][hal]") {

    SECTION("Null Device Basic Operations") {
        auto device = std::make_unique<hal::NullDevice>();

        // Test initialization
        REQUIRE(device->initialize(800, 600));
        REQUIRE(device->getDeviceName() == "Null Render Device (Testing)");
        REQUIRE(device->supportsFeature("textures"));

        // Test operations are recorded
        device->clear(1.0f, 0.0f, 0.0f, 1.0f);
        device->setViewport(0, 0, 800, 600);

        const auto& operations = device->getOperations();
        REQUIRE(!operations.empty());
        REQUIRE(device->hasOperation("clear(1, 0, 0, 1)"));
        REQUIRE(device->hasOperation("setViewport(0, 0, 800, 600)"));

        device->shutdown();
    }

    SECTION("Null Device Resource Management") {
        auto device = std::make_unique<hal::NullDevice>();
        REQUIRE(device->initialize(800, 600));

        // Test texture creation
        unsigned char testData[] = { 255, 0, 0, 255 }; // Red pixel
        uint32_t textureId = device->createTexture(testData, 1, 1, 4);
        REQUIRE(textureId != 0);
        REQUIRE(device->hasOperation("createTexture(id=" + std::to_string(textureId) + ", texture_1x1_4ch)"));

        // Test shader creation
        std::string vertexShader = "vertex shader code";
        std::string fragmentShader = "fragment shader code";
        uint32_t shaderId = device->createShaderProgram(vertexShader, fragmentShader);
        REQUIRE(shaderId != 0);
        REQUIRE(device->hasOperation("createShaderProgram(id=" + std::to_string(shaderId) + ")"));

        // Cleanup
        device->deleteTexture(textureId);
        device->deleteShaderProgram(shaderId);

        device->shutdown();
    }
}

TEST_CASE("New Rendering Architecture - Backend Layer", "[rendering][backend]") {

    SECTION("Backend with Null Device") {
        auto halDevice = std::make_unique<hal::NullDevice>();
        auto* nullDevice = halDevice.get(); // Keep reference for testing

        auto backend = std::make_unique<backend::RenderBackend>(std::move(halDevice));

        // Test initialization
        REQUIRE(backend->initialize(800, 600));
        REQUIRE(backend->isInitialized());
        REQUIRE(backend->getDeviceInfo() == "Null Render Device (Testing)");

        // Test immediate mode rendering
        backend->clear(0.0f, 0.0f, 1.0f, 1.0f);
        REQUIRE(nullDevice->hasOperation("clear(0, 0, 1, 1)"));

        math::Mat4 transform = math::Mat4::identity();
        backend->drawQuad(transform, 1.0f, 0.0f, 0.0f, 1.0f);
        REQUIRE(nullDevice->countOperations("drawQuad") == 1);

        backend->shutdown();
    }

    SECTION("Command Buffer Execution") {
        auto halDevice = std::make_unique<hal::NullDevice>();
        auto* nullDevice = halDevice.get();

        auto backend = std::make_unique<backend::RenderBackend>(std::move(halDevice));
        REQUIRE(backend->initialize(800, 600));

        // Create command buffer
        backend::CommandBuffer commandBuffer;
        commandBuffer.clear(1.0f, 1.0f, 0.0f, 1.0f);
        commandBuffer.setViewport(100, 100, 600, 400);

        math::Mat4 transform = math::Mat4::identity();
        commandBuffer.drawQuad(transform, 0.0f, 1.0f, 0.0f, 1.0f);

        // Execute commands
        backend->executeCommandBuffer(commandBuffer);

        // Verify execution
        REQUIRE(nullDevice->hasOperation("clear(1, 1, 0, 1)"));
        REQUIRE(nullDevice->hasOperation("setViewport(100, 100, 600, 400)"));
        REQUIRE(nullDevice->countOperations("drawQuad") == 1);

        backend->shutdown();
    }

    SECTION("Resource Management") {
        auto halDevice = std::make_unique<hal::NullDevice>();
        auto backend = std::make_unique<backend::RenderBackend>(std::move(halDevice));
        REQUIRE(backend->initialize(800, 600));

        // Test texture creation
        unsigned char testData[] = { 255, 255, 255, 255 };
        backend::TextureCreateInfo texInfo(testData, 2, 2, 4, "test_texture");
        auto textureHandle = backend->createTexture(texInfo);
        REQUIRE(textureHandle != 0);

        // Test resource stats
        auto stats = backend->getResourceStats();
        REQUIRE(stats.textureCount == 1);
        REQUIRE(stats.shaderCount == 0);

        // Cleanup
        backend->deleteTexture(textureHandle);
        stats = backend->getResourceStats();
        REQUIRE(stats.textureCount == 0);

        backend->shutdown();
    }
}

TEST_CASE("New Rendering Architecture - 2D Frontend", "[rendering][2d]") {

    SECTION("2D Renderer Initialization") {
        auto renderer = createNullRenderer2D(800, 600);
        REQUIRE(renderer != nullptr);

        std::string info = renderer->getRendererInfo();
        REQUIRE(info.find("Null Render Device") != std::string::npos);

        const auto& config = renderer->getConfig();
        REQUIRE(config.maxQuadsPerBatch == 1000);
        REQUIRE(config.enableBatching == true);
    }

    SECTION("2D Renderer Camera") {
        auto renderer = createNullRenderer2D(800, 600);
        REQUIRE(renderer != nullptr);

        // Test camera position
        renderer->setCameraPosition(100.0f, 200.0f);
        float x, y;
        renderer->getCameraPosition(x, y);
        REQUIRE(x == 100.0f);
        REQUIRE(y == 200.0f);

        // Test zoom
        renderer->setCameraZoom(2.0f);
        REQUIRE(renderer->getCameraZoom() == 2.0f);

        // Test viewport
        renderer->setViewportSize(1024.0f, 768.0f);
        float w, h;
        renderer->getViewportSize(w, h);
        REQUIRE(w == 1024.0f);
        REQUIRE(h == 768.0f);
    }

    SECTION("2D Renderer Drawing") {
        auto renderer = createNullRenderer2D(800, 600);
        REQUIRE(renderer != nullptr);

        renderer->beginFrame();

        // Test clear
        renderer->clear(twod::Color::BLUE);

        // Test immediate mode drawing
        twod::Rect2D rect(10.0f, 20.0f, 50.0f, 30.0f);
        renderer->drawRect(rect, twod::Color::RED);

        // Test custom transform drawing
        math::Mat4 transform = math::Mat4::translation(100.0f, 150.0f, 0.0f);
        renderer->drawQuad(transform, twod::Color::GREEN);

        renderer->endFrame();

        // Verify operations were recorded (through the HAL)
        auto* backend = renderer->getBackend();
        REQUIRE(backend != nullptr);
        auto* halDevice = backend->getHALDevice();
        REQUIRE(halDevice != nullptr);

        auto* nullDevice = static_cast<hal::NullDevice*>(halDevice);
        REQUIRE(nullDevice->countOperations("clear") >= 1);
        REQUIRE(nullDevice->countOperations("drawQuad") >= 2);
    }

    SECTION("2D Renderer Batching") {
        auto renderer = createNullRenderer2D(800, 600);
        REQUIRE(renderer != nullptr);

        renderer->beginFrame();

        // Test batch rendering
        renderer->beginBatch();

        for (int i = 0; i < 5; i++) {
            twod::Rect2D rect(static_cast<float>(i * 10), 0.0f, 10.0f, 10.0f);
            renderer->batchRect(rect, twod::Color::WHITE, static_cast<float>(i));
        }

        renderer->endBatch();
        renderer->endFrame();

        // Verify batch was rendered
        auto* backend = renderer->getBackend();
        auto* nullDevice = static_cast<hal::NullDevice*>(backend->getHALDevice());
        REQUIRE(nullDevice->countOperations("drawQuad") == 5);
    }
}

TEST_CASE("New Rendering Architecture - Integration", "[rendering][integration]") {

    SECTION("Complete Pipeline Test") {
        auto renderer = createNullRenderer2D(1920, 1080);
        REQUIRE(renderer != nullptr);

        // Create a texture
        unsigned char textureData[] = {
            255, 0, 0, 255,   // Red
            0, 255, 0, 255,   // Green
            0, 0, 255, 255,   // Blue
            255, 255, 0, 255  // Yellow
        };
        auto texture = renderer->createTexture(textureData, 2, 2, 4, "test_pattern");
        REQUIRE(texture != 0);

        // Render a frame with mixed content
        renderer->beginFrame();
        renderer->clear(twod::Color::BLACK);

        // Set camera
        renderer->setCameraPosition(960.0f, 540.0f); // Center of 1920x1080
        renderer->setCameraZoom(1.5f);

        // Draw some immediate mode objects
        twod::Rect2D rect1(0.0f, 0.0f, 100.0f, 100.0f);
        renderer->drawRect(rect1, twod::Color::RED);

        twod::Rect2D rect2(200.0f, 0.0f, 100.0f, 100.0f);
        renderer->drawTexturedRect(rect2, texture);

        // Draw some batched objects
        renderer->beginBatch();
        for (int i = 0; i < 10; i++) {
            twod::Rect2D batchRect(static_cast<float>(i * 20), 200.0f, 15.0f, 15.0f);
            twod::Color color(static_cast<float>(i) / 10.0f, 1.0f - static_cast<float>(i) / 10.0f, 0.5f);
            renderer->batchRect(batchRect, color, static_cast<float>(-i)); // Reverse depth
        }
        renderer->endBatch();

        renderer->endFrame();

        // Verify the complete pipeline worked
        auto stats = renderer->getBackend()->getResourceStats();
        REQUIRE(stats.textureCount >= 1);

        auto* nullDevice = static_cast<hal::NullDevice*>(renderer->getBackend()->getHALDevice());
        REQUIRE(nullDevice->countOperations("clear") >= 1);
        REQUIRE(nullDevice->countOperations("drawQuad") >= 10); // Batched items
        REQUIRE(nullDevice->countOperations("drawTexturedQuad") >= 1);

        // Cleanup
        renderer->deleteTexture(texture);
    }
}

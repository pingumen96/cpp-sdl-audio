#include <iostream>
#include "./scene/Renderer2DIntegration.h"

using namespace scene;

/**
 * @brief Demo del sistema Renderer2D
 *
 * Questo file dimostra come utilizzare il nuovo sistema Renderer2D
 * integrato con l'architettura esistente. È progettato per essere
 * un esempio pratico di utilizzo.
 */

void printHeader() {
    std::cout << "========================================" << std::endl;
    std::cout << "         Renderer2D Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

void demonstrateBasicUsage() {
    std::cout << "=== Basic Usage Demo ===" << std::endl;

    // Crea l'esempio di integrazione
    Renderer2DIntegrationExample example;

    if (!example.init(1920, 1080)) {
        std::cerr << "Failed to initialize Renderer2D system!" << std::endl;
        return;
    }

    std::cout << "✓ System initialized successfully" << std::endl;

    // Renderizza alcuni frame per mostrare il funzionamento
    std::cout << "Rendering test frames..." << std::endl;

    for (int frame = 0; frame < 10; ++frame) {
        example.renderFrame();

        if (frame % 5 == 0) {
            std::cout << "  Frame " << frame << " rendered" << std::endl;
        }
    }

    std::cout << "✓ Test frames rendered successfully" << std::endl;

    example.shutdown();
    std::cout << "✓ System shutdown cleanly" << std::endl;
    std::cout << std::endl;
}

void demonstrateManualRendering() {
    std::cout << "=== Manual Rendering Demo ===" << std::endl;

    Renderer2DIntegrationExample example;

    if (!example.init()) {
        std::cerr << "Failed to initialize system!" << std::endl;
        return;
    }

    // Ottieni riferimenti ai componenti
    auto* renderer2D = example.getRenderer2D();
    auto* testSystem = example.getTestSystem();

    if (!renderer2D || !testSystem) {
        std::cerr << "Failed to get renderer components!" << std::endl;
        example.shutdown();
        return;
    }

    // Crea una scena personalizzata
    testSystem->clearSprites();

    // Aggiungi alcuni sprite di test
    testSystem->addTestSprite(
        math::Vec2f(-300.0f, 0.0f),
        math::Vec2f(200.0f, 100.0f),
        Color::Red
    );

    testSystem->addTestSprite(
        math::Vec2f(0.0f, 0.0f),
        math::Vec2f(150.0f, 150.0f),
        Color::Green
    );

    testSystem->addTestSprite(
        math::Vec2f(200.0f, -100.0f),
        math::Vec2f(100.0f, 200.0f),
        Color::Blue
    );

    std::cout << "✓ Custom scene created" << std::endl;

    // Renderizza la scena personalizzata
    std::cout << "Rendering custom scene..." << std::endl;
    example.renderFrame();

    // Mostra le statistiche
    auto stats = renderer2D->getStats();
    std::cout << "  Quads rendered: " << stats.quadCount << std::endl;
    std::cout << "  Batches created: " << stats.batchCount << std::endl;
    std::cout << "  Texture binds: " << stats.textureBinds << std::endl;

    example.shutdown();
    std::cout << "✓ Manual rendering demo complete" << std::endl;
    std::cout << std::endl;
}

void demonstrateCameraMovement() {
    std::cout << "=== Camera Movement Demo ===" << std::endl;

    Renderer2DIntegrationExample example;

    if (!example.init()) {
        std::cerr << "Failed to initialize system!" << std::endl;
        return;
    }

    auto* testSystem = example.getTestSystem();
    if (!testSystem) {
        std::cerr << "Failed to get test system!" << std::endl;
        example.shutdown();
        return;
    }

    // Crea una scena con pattern a griglia
    testSystem->clearSprites();

    for (int x = -5; x <= 5; ++x) {
        for (int y = -3; y <= 3; ++y) {
            Color color = ((x + y) % 2 == 0) ? Color::White : Color::Black;
            testSystem->addTestSprite(
                math::Vec2f(x * 100.0f, y * 100.0f),
                math::Vec2f(80.0f, 80.0f),
                color
            );
        }
    }

    std::cout << "✓ Grid pattern created" << std::endl;
    std::cout << "Animating camera movement..." << std::endl;

    // Anima la camera
    auto& camera = testSystem->getCamera();

    for (int frame = 0; frame < 20; ++frame) {
        // Movimento circolare della camera
        float t = frame * 0.3f;
        math::Vec2f newPos(
            std::cos(t) * 200.0f,
            std::sin(t) * 150.0f
        );

        camera.setPosition(newPos);

        // Zoom oscillante
        float zoom = 1.0f + 0.3f * std::sin(t * 2.0f);
        camera.setZoom(zoom);

        example.renderFrame();

        if (frame % 5 == 0) {
            std::cout << "  Frame " << frame << " - Camera pos: ("
                << newPos.x() << ", " << newPos.y()
                << "), Zoom: " << zoom << std::endl;
        }
    }

    example.shutdown();
    std::cout << "✓ Camera movement demo complete" << std::endl;
    std::cout << std::endl;
}

void demonstratePerformanceTest() {
    std::cout << "=== Performance Test Demo ===" << std::endl;

    Renderer2DIntegrationExample example;

    if (!example.init()) {
        std::cerr << "Failed to initialize system!" << std::endl;
        return;
    }

    auto* testSystem = example.getTestSystem();
    if (!testSystem) {
        std::cerr << "Failed to get test system!" << std::endl;
        example.shutdown();
        return;
    }

    // Crea molti sprite per testare le performance
    testSystem->clearSprites();

    const int numSprites = 1000;
    std::cout << "Creating " << numSprites << " sprites..." << std::endl;

    for (int i = 0; i < numSprites; ++i) {
        float x = (i % 40) * 25.0f - 500.0f;
        float y = (i / 40) * 25.0f - 300.0f;

        Color color;
        switch (i % 4) {
        case 0: color = Color::Red; break;
        case 1: color = Color::Green; break;
        case 2: color = Color::Blue; break;
        case 3: color = Color::Yellow; break;
        }

        testSystem->addTestSprite(
            math::Vec2f(x, y),
            math::Vec2f(20.0f, 20.0f),
            color
        );
    }

    std::cout << "✓ " << numSprites << " sprites created" << std::endl;
    std::cout << "Running performance test..." << std::endl;

    // Renderizza alcuni frame e misura le performance
    for (int frame = 0; frame < 5; ++frame) {
        example.renderFrame();

        auto* renderer2D = example.getRenderer2D();
        if (renderer2D) {
            auto stats = renderer2D->getStats();
            std::cout << "  Frame " << frame << " - Quads: " << stats.quadCount
                << ", Batches: " << stats.batchCount << std::endl;
        }
    }

    example.shutdown();
    std::cout << "✓ Performance test complete" << std::endl;
    std::cout << std::endl;
}

int main() {
    printHeader();

    try {
        demonstrateBasicUsage();
        demonstrateManualRendering();
        demonstrateCameraMovement();
        demonstratePerformanceTest();

        std::cout << "========================================" << std::endl;
        std::cout << "All demos completed successfully!" << std::endl;
        std::cout << "========================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Demo failed with unknown exception!" << std::endl;
        return 1;
    }

    return 0;
}

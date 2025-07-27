#pragma once

#include "rendering/Renderer2D.h"
#include "../math/math.h"
#include <vector>

namespace scene {

    /**
     * @brief Sistema semplificato per test senza ECS
     *
     * Utile per testare il Renderer2D senza dover configurare
     * un intero sistema ECS. Questo sostituisce il vecchio SimpleRenderTest
     * che era duplicato in SpriteRenderSystem.h
     */
    class TestRenderer2D {
    private:
        IRenderer2D* renderer2D = nullptr;
        Camera2D camera;

        struct TestSprite {
            math::Vec2f position;
            math::Vec2f size;
            Color color;
            TextureHandle texture;
            float rotation = 0.0f;
        };

        std::vector<TestSprite> testSprites;

    public:
        TestRenderer2D(IRenderer2D& renderer) : renderer2D(&renderer) {
            // Setup camera
            camera.setViewportSize(math::Vec2f(1920.0f, 1080.0f));
            camera.setPosition(math::Vec2f(0.0f, 0.0f));
            camera.setZoom(1.0f);
        }

        /**
         * @brief Aggiungi sprite di test
         */
        void addTestSprite(const math::Vec2f& pos, const math::Vec2f& size,
            const Color& color, const TextureHandle& texture = "") {
            testSprites.push_back({ pos, size, color, texture, 0.0f });
        }

        /**
         * @brief Render di tutti gli sprite di test
         */
        void render() {
            if (!renderer2D) {
                return;
            }

            renderer2D->beginScene(camera);

            for (const auto& sprite : testSprites) {
                Rect2D rect(sprite.position, sprite.size);
                renderer2D->drawRect(rect, sprite.color, sprite.texture);
            }

            renderer2D->endScene();
        }

        /**
         * @brief Accesso alla camera per modifiche
         */
        Camera2D& getCamera() { return camera; }
        const Camera2D& getCamera() const { return camera; }

        /**
         * @brief Clear di tutti gli sprite
         */
        void clearSprites() {
            testSprites.clear();
        }

        /**
         * @brief Crea una scena di test con alcuni rettangoli
         */
        void createTestScene() {
            clearSprites();

            // Sfondo
            addTestSprite(math::Vec2f(0.0f, 0.0f), math::Vec2f(1920.0f, 1080.0f), Color::Blue);

            // Rettangoli colorati
            addTestSprite(math::Vec2f(-200.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Red);
            addTestSprite(math::Vec2f(0.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Green);
            addTestSprite(math::Vec2f(200.0f, 0.0f), math::Vec2f(100.0f, 100.0f), Color::Yellow);

            // Rettangoli più piccoli
            addTestSprite(math::Vec2f(-100.0f, 150.0f), math::Vec2f(50.0f, 50.0f), Color::Magenta);
            addTestSprite(math::Vec2f(100.0f, 150.0f), math::Vec2f(50.0f, 50.0f), Color::Cyan);
        }
    };

} // namespace scene

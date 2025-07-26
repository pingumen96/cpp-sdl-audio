#pragma once

#include "../rendering/Renderer2D.h"
#include "../../ecs/ComponentArray.h"
#include "../../ecs/components/CommonComponents.h"
#include <memory>

namespace scene {

    /**
     * @brief Componente per sprite 2D
     */
    struct SpriteComponent {
        Color color = Color::White;
        TextureHandle texture;
        math::Vec2f size{ 1.0f, 1.0f };
        uint32_t layer = 0;
        float depth = 0.0f;
        bool visible = true;

        SpriteComponent() = default;
        SpriteComponent(const Color& col, const math::Vec2f& sz = math::Vec2f(1.0f))
            : color(col), size(sz) {}
        SpriteComponent(const TextureHandle& tex, const math::Vec2f& sz = math::Vec2f(1.0f))
            : texture(tex), size(sz) {}
    };

    /**
     * @brief Sistema ECS per il rendering di sprite 2D
     *
     * Questo sistema itera su tutte le entità che hanno Transform e SpriteComponent
     * e le invia al Renderer2D per il rendering.
     */
    class SpriteRenderSystem {
    private:
        IRenderer2D* renderer2D = nullptr;

        // Riferimenti ai ComponentArray (dovrai adattarli al tuo sistema ECS)
        // ecs::ComponentArray<ecs::components::Transform>* transforms = nullptr;
        // ecs::ComponentArray<SpriteComponent>* sprites = nullptr;

        bool enabled = true;

    public:
        SpriteRenderSystem() = default;
        ~SpriteRenderSystem() = default;

        /**
         * @brief Inizializza il sistema con il renderer 2D
         */
        void init(IRenderer2D& renderer) {
            renderer2D = &renderer;
        }

        /**
         * @brief Aggiorna il sistema (chiama questo durante il render loop)
         */
        void update(/* ECS Manager& ecsManager */) {
            if (!renderer2D || !enabled) {
                return;
            }

            // Esempio di come potrebbe funzionare con il tuo sistema ECS
            // Dovrai adattarlo alla tua implementazione specifica

            /*
            // Itera su tutte le entità con Transform e SpriteComponent
            for (auto entity : ecsManager.getEntitiesWithComponents<Transform, SpriteComponent>()) {
                const auto& transform = ecsManager.getComponent<Transform>(entity);
                const auto& sprite = ecsManager.getComponent<SpriteComponent>(entity);

                if (!sprite.visible) {
                    continue;
                }

                // Converte Transform in Rect2D
                Rect2D rect;
                rect.position = math::Vec2f(transform.position.x(), transform.position.y());
                rect.size = sprite.size;

                // Invia al renderer
                renderer2D->drawRect(rect, sprite.color, sprite.texture);
            }
            */
        }

        /**
         * @brief Abilita/disabilita il sistema
         */
        void setEnabled(bool enable) {
            enabled = enable;
        }

        /**
         * @brief Verifica se il sistema è abilitato
         */
        bool isEnabled() const {
            return enabled;
        }

        /**
         * @brief Esempio di metodo helper per creare sprite
         */
         /*
         ecs::Entity createSprite(ECSManager& ecsManager,
                                const math::Vec2f& position,
                                const math::Vec2f& size,
                                const Color& color,
                                const TextureHandle& texture = "") {
             auto entity = ecsManager.createEntity();

             // Aggiungi Transform
             Transform transform;
             transform.position = math::Vec3f(position.x(), position.y(), 0.0f);
             ecsManager.addComponent(entity, transform);

             // Aggiungi SpriteComponent
             SpriteComponent sprite(color, size);
             sprite.texture = texture;
             ecsManager.addComponent(entity, sprite);

             return entity;
         }
         */
    };

    /**
     * @brief Sistema semplificato per test senza ECS
     *
     * Utile per testare il Renderer2D senza dover configurare
     * un intero sistema ECS.
     */
    class SimpleRenderTest {
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
        SimpleRenderTest(IRenderer2D& renderer) : renderer2D(&renderer) {
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

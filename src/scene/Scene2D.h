#pragma once
/*
 * Scene2D.h ― versione pulita e 100 % ECS‑oriented
 *
 * Principi:
 *  • La scena NON conserva logica di gioco, input o stato di sistemi.
 *  • Si limita a:
 *      – creare le risorse grafiche (Renderer2D);
 *      – registrare i sistemi ECS generici necessari;
 *      – fornire utility per istanziare entità 2D (Transform + Renderable2D).
 *  • Le vistose stampe di debug e i puntatori a sistemi sono stati rimossi:
 *      le scene non devono conoscere l’ordine di esecuzione dei sistemi.
 */

#include "Scene.h"
#include "rendering/Renderer2D.h"
#include "rendering/Renderer2DImpl.h"

#include "../ecs/Coordinator.h"
#include "../ecs/components/Renderable2D.h"
#include "../ecs/components/CommonComponents.h"
#include "../ecs/systems/Renderer2DSystem.h"
#include "../ecs/systems/CommonSystems.h"

namespace scene {

    class Scene2D : public Scene {
    protected:
        /*  Unica risorsa di cui la scena si cura direttamente.
            Viene passata al sistema Renderer2DSystem tramite riferimento raw
            (l’ECS gestirà il ciclo di vita del sistema).                         */
        std::unique_ptr<IRenderer2D> renderer2D{};

    public:
        Scene2D() = default;
        ~Scene2D() override = default;

        /*--------------------------------------------------------------------------
         * INITIALISATION
         *--------------------------------------------------------------------------*/
        void initialize2D(SceneManager& manager) {
            auto* coord = getCoordinator();
            if (!coord) return;

            /*‑‑ Crea l’implementazione concreta del Renderer2D ‑‑*/
            renderer2D = std::make_unique<Renderer2DImpl>(manager.getRenderBackend());

            /*‑‑ REGISTRA I SISTEMI COMUNI ‑‑*/
            // Physica
            {
                auto sys = coord->registerSystem<ecs::systems::PhysicsSystem>();
                ecs::Signature sig;
                sig.set(coord->getComponentType<ecs::components::Transform>());
                sig.set(coord->getComponentType<ecs::components::Velocity>());
                coord->setSystemSignature<ecs::systems::PhysicsSystem>(sig);
            }
            // Salute
            {
                auto sys = coord->registerSystem<ecs::systems::HealthSystem>();
                ecs::Signature sig;
                sig.set(coord->getComponentType<ecs::components::Health>());
                coord->setSystemSignature<ecs::systems::HealthSystem>(sig);
            }
            // Rendering 2D
            {
                auto sys = coord->registerSystem<ecs::systems::Renderer2DSystem>(renderer2D.get());
                ecs::Signature sig;
                sig.set(coord->getComponentType<ecs::components::Transform>());
                sig.set(coord->getComponentType<ecs::components::Renderable2D>());
                coord->setSystemSignature<ecs::systems::Renderer2DSystem>(sig);
            }
        }

        /*--------------------------------------------------------------------------
         * RENDERING
         *--------------------------------------------------------------------------*/
        void render(RenderQueueBuilder& builder) override {
            if (!renderer2D) return;

            // Imposta la camera ortografica secondo la viewport corrente
            Camera2D cam;
            cam.setPosition({ 0.f, 0.f });

            renderer2D->beginScene(cam);

            /* Il sistema Renderer2DSystem verrà aggiornato dal loop ECS globale,
               quindi qui non c’è nulla da fare (niente update manuale).           */

            render2DCustom();       // Hook opzionale per disegni manuali

            renderer2D->endScene();
        }

        /* Facoltativo: override per effetti particolari fra beginScene/endScene */
        virtual void render2DCustom() {}

        /*--------------------------------------------------------------------------
         * ENTITY HELPERS
         *--------------------------------------------------------------------------*/
        ecs::Entity createSprite(const math::Vec2f& pos,
            const math::Vec2f& size,
            const std::string& tex,
            const scene::Color& tint = scene::Color::White,
            uint32_t layer = 0) {
            auto* coord = getCoordinator();
            if (!coord) return 0;

            auto e = coord->createEntity();

            ecs::components::Transform t;
            t.position = { pos.x(), pos.y(), static_cast<float>(layer) };
            t.scale = { size.x(), size.y(), 1.f };
            coord->addComponent(e, t);

            ecs::components::Renderable2D r;
            r.textureId = tex;
            r.color = tint;
            coord->addComponent(e, r);

            return e;
        }

        ecs::Entity createColoredQuad(const math::Vec2f& pos,
            const math::Vec2f& size,
            const scene::Color& color,
            uint32_t layer = 0) {
            return createSprite(pos, size, /*texture*/"", color, layer);
        }

        /* Accessor comodo se proprio serve un draw immediato (debug, gizmo) */
        IRenderer2D* getRenderer2D() const { return renderer2D.get(); }
    };

} // namespace scene

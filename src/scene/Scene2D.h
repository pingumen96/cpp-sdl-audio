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

#include "SceneManager.h"
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

        /* INITIALISATION: dichiarazione */
        void initialize2D(SceneManager& manager);

        /* RENDERING */
        void render(RenderQueueBuilder& builder) override;

        /* Facoltativo: override per effetti particolari */
        virtual void render2DCustom() {}

        /* ENTITY HELPERS */
        ecs::Entity createSprite(const math::Vec2f& pos,
            const math::Vec2f& size,
            const std::string& tex,
            const scene::Color& tint = scene::Color::White,
            uint32_t layer = 0);

        ecs::Entity createColoredQuad(const math::Vec2f& pos,
            const math::Vec2f& size,
            const scene::Color& color,
            uint32_t layer = 0);

        IRenderer2D* getRenderer2D() const { return renderer2D.get(); }
    };
} // namespace scene

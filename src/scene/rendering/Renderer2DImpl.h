#pragma once

#include "Renderer2D.h"
#include "QuadBatch.h"
#include "Shader2DLibrary.h"
#include "VertexCache2D.h"
#include "Material2D.h"
#include "RenderQueueBuilder.h"
#include "../../resources/ResourceManager.h"
#include <memory>

namespace scene {

    /**
     * @brief Implementazione concreta del renderer 2D
     *
     * Implementa IRenderer2D senza conoscenza del backend specifico.
     * Si limita a impacchettare i dati in QuadCommand e a inserirli
     * in un QuadBatch per il batching automatico.
     */
    class Renderer2DImpl : public IRenderer2D {
    private:
        // Dipendenze esterne
        resources::ResourceManager* resourceManager = nullptr;
        RenderQueueBuilder* renderQueueBuilder = nullptr;

        // Componenti interni
        std::unique_ptr<QuadBatch> quadBatch;
        std::unique_ptr<VertexCache2D> vertexCache;

        // Configurazione
        RendererConfig2D config;

        // Stato corrente
        Camera2D currentCamera;
        bool sceneActive = false;
        bool initialized = false;

        // Statistiche
        mutable Stats currentStats;

    public:
        Renderer2DImpl(resources::ResourceManager& resMgr, RenderQueueBuilder& builder)
            : resourceManager(&resMgr), renderQueueBuilder(&builder) {}

        virtual ~Renderer2DImpl() {
            shutdown();
        }

        /**
         * @brief Inizializza il renderer
         */
        bool init(const RendererConfig2D& configuration) override {
            if (initialized) {
                return true;
            }

            config = configuration;

            // Inizializza la libreria shader
            if (!Shader2DLibrary::getInstance().init(*resourceManager)) {
                return false;
            }

            // Inizializza la cache dei vertici
            vertexCache = std::make_unique<VertexCache2D>();
            if (!vertexCache->init(*resourceManager)) {
                return false;
            }

            // Inizializza il quad batch
            quadBatch = std::make_unique<QuadBatch>(config.maxQuadsPerBatch, config.enableSorting);
            quadBatch->setUnitQuadHandle(vertexCache->getUnitQuadHandle());

            initialized = true;
            return true;
        }

        /**
         * @brief Inizia una nuova scena
         */
        void beginScene(const Camera2D& camera) override {
            if (!initialized) {
                return;
            }

            currentCamera = camera;
            sceneActive = true;

            // Reset del batch per il nuovo frame
            quadBatch->reset();

            // Reset delle statistiche
            currentStats = Stats{};
        }

        /**
         * @brief Disegna un rettangolo
         */
        void drawRect(const Rect2D& rect, const Color& color,
            const TextureHandle& texture = "") override {
            if (!sceneActive || !initialized) {
                return;
            }

            // Crea il comando quad dal rettangolo
            QuadCommand command = QuadCommand::fromRect(
                rect.position,
                rect.size,
                color.toVec4(),
                texture,
                0.0f,  // rotation (per ora sempre 0)
                0,     // layer (per ora sempre 0)
                0.0f   // depth (per ora sempre 0)
            );

            // Aggiunge al batch
            quadBatch->push(command);

            // Aggiorna statistiche
            currentStats.quadCount++;
        }

        /**
         * @brief Termina la scena e invia i comandi
         */
        void endScene() override {
            if (!sceneActive || !initialized) {
                return;
            }

            // Imposta la camera nel command buffer tramite il builder
            CameraParams cameraParams;
            cameraParams.viewMatrix = currentCamera.getViewMatrix();
            cameraParams.projectionMatrix = currentCamera.getProjectionMatrix();
            cameraParams.position = math::Vec3f(currentCamera.getPosition().x(),
                currentCamera.getPosition().y(), 0.0f);

            // Flush del batch al render queue builder
            quadBatch->flush(*renderQueueBuilder);

            // Aggiorna statistiche dal batch
            auto batchStats = quadBatch->getStats();
            currentStats.batchCount = batchStats.batchCount;
            currentStats.textureBinds = batchStats.batchCount; // Approssimazione

            sceneActive = false;
        }

        /**
         * @brief Shutdown del renderer
         */
        void shutdown() override {
            if (!initialized) {
                return;
            }

            quadBatch.reset();
            vertexCache.reset();

            sceneActive = false;
            initialized = false;
        }

        /**
         * @brief Ottieni statistiche di rendering
         */
        Stats getStats() const override {
            return currentStats;
        }

        /**
         * @brief Reset delle statistiche
         */
        void resetStats() override {
            currentStats = Stats{};
        }

        // --- Metodi di utilità aggiuntivi ---

        /**
         * @brief Disegna un rettangolo con rotazione
         */
        void drawRect(const Rect2D& rect, const Color& color, float rotation,
            const TextureHandle& texture = "", uint32_t layer = 0, float depth = 0.0f) override {
            if (!sceneActive || !initialized) {
                return;
            }

            QuadCommand command = QuadCommand::fromRect(
                rect.position,
                rect.size,
                color.toVec4(),
                texture,
                rotation,
                layer,
                depth
            );

            quadBatch->push(command);
            currentStats.quadCount++;
        }

        /**
         * @brief Disegna un quadrato (size uguale per width e height)
         */
        void drawSquare(const math::Vec2f& position, float size, const Color& color,
            const TextureHandle& texture = "") {
            drawRect(Rect2D(position, math::Vec2f(size, size)), color, texture);
        }

        /**
         * @brief Disegna una linea come rettangolo sottile
         */
        void drawLine(const math::Vec2f& start, const math::Vec2f& end,
            float thickness, const Color& color) {
            math::Vec2f direction = end - start;
            float length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

            if (length < 0.001f) {
                return; // Linea troppo corta
            }

            // Calcola rotazione
            float angle = std::atan2(direction.y(), direction.x());

            // Centro della linea
            math::Vec2f center = start + direction * 0.5f;

            drawRect(Rect2D(center, math::Vec2f(length, thickness)), color, angle);
        }        /**
         * @brief Verifica se è in una scena attiva
         */
        bool isInScene() const {
            return sceneActive;
        }

        /**
         * @brief Verifica se è inizializzato
         */
        bool isInitialized() const {
            return initialized;
        }

        /**
         * @brief Ottieni la configurazione corrente
         */
        const RendererConfig2D& getConfig() const {
            return config;
        }

        /**
         * @brief Ottieni la camera corrente
         */
        const Camera2D& getCurrentCamera() const {
            return currentCamera;
        }
    };

    /**
     * @brief Factory per creare il renderer 2D
     */
    inline IRenderer2DPtr createRenderer2D(resources::ResourceManager& resMgr,
        RenderQueueBuilder& builder) {
        return std::make_unique<Renderer2DImpl>(resMgr, builder);
    }

} // namespace scene

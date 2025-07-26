#pragma once

#include "../../resources/ResourceManager.h"
#include <string>
#include <memory>

namespace scene {

    /**
     * @brief Cache per la geometria del quad unitario
     *
     * Tiene un unico VAO/VBO statico con la geometria "unit quad" (2 triangoli).
     * Lo stesso buffer è riusato da tutti i draw call tramite instancing o
     * offset via matrice di trasformazione.
     */
    class VertexCache2D {
    private:
        std::string unitQuadMeshHandle;
        resources::ResourceManager* resourceManager = nullptr;
        bool initialized = false;

    public:
        VertexCache2D() = default;
        ~VertexCache2D() = default;

        /**
         * @brief Inizializza la cache con il resource manager
         */
        bool init(resources::ResourceManager& resMgr) {
            if (initialized) {
                return true;
            }

            resourceManager = &resMgr;

            // Crea la geometria del quad unitario
            if (!createUnitQuad()) {
                return false;
            }

            initialized = true;
            return true;
        }

        /**
         * @brief Ottieni l'handle del quad unitario
         */
        const std::string& getUnitQuadHandle() const {
            return unitQuadMeshHandle;
        }

        /**
         * @brief Verifica se è inizializzato
         */
        bool isInitialized() const {
            return initialized;
        }

    private:
        /**
         * @brief Crea la geometria del quad unitario
         */
        bool createUnitQuad() {
            if (!resourceManager) {
                return false;
            }

            // Quad unitario centrato nell'origine con coordinate [-0.5, 0.5]
            // Questo permette di applicare scale e translation facilmente

            // Vertici del quad (posizione + UV)
            struct Vertex {
                float x, y, z;    // Posizione
                float u, v;       // Coordinate texture
            };

            std::vector<Vertex> vertices = {
                // Vertice     // UV
                {-0.5f, -0.5f, 0.0f,  0.0f, 0.0f},  // Bottom-left
                { 0.5f, -0.5f, 0.0f,  1.0f, 0.0f},  // Bottom-right
                { 0.5f,  0.5f, 0.0f,  1.0f, 1.0f},  // Top-right
                {-0.5f,  0.5f, 0.0f,  0.0f, 1.0f}   // Top-left
            };

            // Indici per 2 triangoli
            std::vector<uint32_t> indices = {
                0, 1, 2,  // Primo triangolo
                2, 3, 0   // Secondo triangolo
            };

            // Qui dovresti usare l'API del tuo ResourceManager per creare
            // effettivamente la mesh. Dato che non conosco l'API esatta,
            // assumo che esista qualcosa come:
            //
            // unitQuadMeshHandle = resourceManager->createMeshFromData(
            //     "unit_quad_2d", vertices, indices);
            //
            // Per ora assegno un handle placeholder
            unitQuadMeshHandle = "unit_quad_2d";

            // Qui dovrai implementare la logica specifica per il tuo ResourceManager
            // per caricare la geometria del quad

            return !unitQuadMeshHandle.empty();
        }
    };

} // namespace scene

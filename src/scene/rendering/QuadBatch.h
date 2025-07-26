#pragma once

#include "QuadCommand.h"
#include "Material2D.h"
#include "RenderQueueBuilder.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace scene {

    /**
     * @brief Batch per raggruppare quad compatibili
     *
     * Raccoglie QuadCommand temporaneamente, li raggruppa per compatibilità
     * (stesso materiale/texture) e li converte in DrawItem per il
     * RenderQueueBuilder.
     */
    class QuadBatch {
    private:
        struct Batch {
            Material2D material;
            std::vector<QuadCommand> commands;
            uint32_t layer = 0;

            Batch() = default;
            Batch(const Material2D& mat, uint32_t lay) : material(mat), layer(lay) {}
        };

        std::vector<Batch> batches;
        uint32_t maxQuadsPerBatch;
        bool enableSorting;
        std::string unitQuadMeshHandle;

        // Statistiche
        mutable uint32_t totalQuads = 0;
        mutable uint32_t totalBatches = 0;

    public:
        QuadBatch(uint32_t maxQuads = 1000, bool sorting = true)
            : maxQuadsPerBatch(maxQuads), enableSorting(sorting) {}

        /**
         * @brief Imposta l'handle del quad unitario
         */
        void setUnitQuadHandle(const std::string& handle) {
            unitQuadMeshHandle = handle;
        }

        /**
         * @brief Aggiungi un comando quad al batch
         */
        void push(const QuadCommand& command) {
            // Trova il batch compatibile o ne crea uno nuovo
            Batch* targetBatch = findCompatibleBatch(command);

            if (!targetBatch || targetBatch->commands.size() >= maxQuadsPerBatch) {
                // Crea un nuovo batch
                Material2D material = createMaterialFromCommand(command);
                batches.emplace_back(material, command.layer);
                targetBatch = &batches.back();
            }

            targetBatch->commands.push_back(command);
        }

        /**
         * @brief Flush tutti i batch al RenderQueueBuilder
         */
        void flush(RenderQueueBuilder& builder) {
            if (batches.empty()) {
                return;
            }

            // Ordina i batch per layer se richiesto
            if (enableSorting) {
                sortBatches();
            }

            // Per ogni batch, crea i DrawItem e inviali al builder
            for (const auto& batch : batches) {
                emitBatch(batch, builder);
            }

            // Aggiorna statistiche
            totalQuads = getTotalQuadCount();
            totalBatches = batches.size();

            // Pulisci per il prossimo frame
            reset();
        }

        /**
         * @brief Reset per il prossimo frame
         */
        void reset() {
            batches.clear();
        }

        /**
         * @brief Ottieni statistiche
         */
        struct Stats {
            uint32_t quadCount = 0;
            uint32_t batchCount = 0;
        };

        Stats getStats() const {
            return { totalQuads, totalBatches };
        }

    private:
        /**
         * @brief Trova un batch compatibile per il comando
         */
        Batch* findCompatibleBatch(const QuadCommand& command) {
            Material2D commandMaterial = createMaterialFromCommand(command);

            for (auto& batch : batches) {
                if (batch.layer == command.layer &&
                    batch.material.isBatchCompatible(commandMaterial)) {
                    return &batch;
                }
            }

            return nullptr;
        }

        /**
         * @brief Crea un materiale dal comando
         */
        Material2D createMaterialFromCommand(const QuadCommand& command) const {
            if (command.textureId.empty()) {
                return Material2D::createSolidColor(command.color);
            } else {
                return Material2D::createTexturedTinted(command.textureId, command.color);
            }
        }

        /**
         * @brief Ordina i batch per layer e profondità
         */
        void sortBatches() {
            std::sort(batches.begin(), batches.end(),
                [](const Batch& a, const Batch& b) {
                    if (a.layer != b.layer) {
                        return a.layer < b.layer;
                    }

                    // Se stesso layer, ordina per profondità media
                    float avgDepthA = getAverageDepth(a);
                    float avgDepthB = getAverageDepth(b);
                    return avgDepthA < avgDepthB;
                });

            // Ordina anche i comandi all'interno di ogni batch
            for (auto& batch : batches) {
                if (enableSorting) {
                    std::sort(batch.commands.begin(), batch.commands.end(),
                        [](const QuadCommand& a, const QuadCommand& b) {
                            return a.depth < b.depth;
                        });
                }
            }
        }

        /**
         * @brief Calcola la profondità media di un batch
         */
        static float getAverageDepth(const Batch& batch) {
            if (batch.commands.empty()) {
                return 0.0f;
            }

            float sum = 0.0f;
            for (const auto& cmd : batch.commands) {
                sum += cmd.depth;
            }
            return sum / batch.commands.size();
        }

        /**
         * @brief Emette un batch come DrawItem
         */
        void emitBatch(const Batch& batch, RenderQueueBuilder& builder) const {
            if (batch.commands.empty()) {
                return;
            }

            // Per ora, creiamo un DrawItem per ogni quad nel batch
            // In futuro si potrebbe implementare l'instancing vero
            for (const auto& command : batch.commands) {
                DrawItem item;
                item.modelMatrix = command.transform;
                item.meshId = unitQuadMeshHandle;
                item.materialId = batch.material.getBatchId();
                item.renderLayer = batch.layer;
                item.depth = command.depth;

                builder.enqueue(item);
            }
        }

        /**
         * @brief Conta il numero totale di quad
         */
        uint32_t getTotalQuadCount() const {
            uint32_t count = 0;
            for (const auto& batch : batches) {
                count += static_cast<uint32_t>(batch.commands.size());
            }
            return count;
        }
    };

} // namespace scene

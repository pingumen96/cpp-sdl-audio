#pragma once

#include "ECSTypes.h"

namespace ecs {

    /**
     * @brief Abstract interface for component arrays
     *
     * This interface allows the ComponentManager to notify all component arrays
     * when an entity is destroyed, so they can clean up their data accordingly.
     */
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;

        /**
         * @brief Called when an entity is destroyed
         * @param entity The entity that was destroyed
         */
        virtual void entityDestroyed(Entity entity) = 0;
    };

} // namespace ecs

#pragma once

#include <bitset>
#include <cstdint>

namespace ecs {

    /**
     * @brief Maximum number of entities that can exist simultaneously
     */
    static constexpr size_t MAX_ENTITIES = 5000;

    /**
     * @brief Maximum number of component types that can be registered
     */
    static constexpr size_t MAX_COMPONENTS = 32;

    /**
     * @brief Unique identifier for an entity
     */
    using Entity = std::uint32_t;

    /**
     * @brief Component type identifier
     */
    using ComponentType = std::uint8_t;

    /**
     * @brief Bitset representing which components an entity possesses
     */
    using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace ecs

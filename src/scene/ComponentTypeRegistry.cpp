#include "ComponentTypeRegistry.h"

namespace scene {

    // Define static member variables
    std::unordered_map<std::type_index, ecs::ComponentType> ComponentTypeRegistry::globalTypes;
    ecs::ComponentType ComponentTypeRegistry::nextTypeId = 0;
    bool ComponentTypeRegistry::initialized = false;
    std::mutex ComponentTypeRegistry::registryMutex;

}

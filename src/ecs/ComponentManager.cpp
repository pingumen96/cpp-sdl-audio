#include "ComponentManager.h"
#include "../scene/ComponentTypeRegistry.h"

namespace ecs {

    ComponentType ComponentManager::getGlobalComponentType(std::type_index typeIndex) {
        return scene::ComponentTypeRegistry::getOrRegisterTypeByIndex(typeIndex);
    }

}

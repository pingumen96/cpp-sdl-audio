#include "Scene.h"
#include "SceneManager.h"

namespace scene {

    bool Scene::isResourceBundleReady(SceneManager& manager) const {
        if (auto* resourceManager = getResourceManager(manager)) {
            return resourceBundle.isReady(*resourceManager);
        }
        return resourceBundle.isEmpty(); // Empty bundle is considered ready
    }

    float Scene::getResourceBundleProgress(SceneManager& manager) const {
        if (auto* resourceManager = getResourceManager(manager)) {
            return resourceBundle.getProgress(*resourceManager);
        }
        return 1.0f; // Empty bundle is 100% loaded
    }

    resources::ResourceManager* Scene::getResourceManager(SceneManager& manager) const {
        return manager.getResourceManager();
    }

} // namespace scene

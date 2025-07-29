#include "SceneManager.h"
#include "SceneTransitions.h"
#include "rendering/NullBackend.h"
#include "rendering/Renderer2DImpl.h"
#include "../math/math.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace scene {

    SceneManager::SceneManager(RenderBackendPtr backend)
        : renderBackend(std::move(backend)) {

        // Create default resource manager
        resourceManager = resources::createDefaultResourceManager();

        // Create default null backend if none provided
        if (!renderBackend) {
            renderBackend = std::make_unique<NullBackend>();
        }
    }

    SceneManager::~SceneManager() {
        shutdown();
    }

    bool SceneManager::initialize(uint32_t width, uint32_t height) {
        if (initialized) {
            std::cerr << "[SceneManager] Already initialized!" << std::endl;
            return false;
        }

        renderWidth = width;
        renderHeight = height;

        // Initialize render backend
        if (!renderBackend->init(width, height)) {
            std::cerr << "[SceneManager] Failed to initialize render backend!" << std::endl;
            return false;
        }

        // Initialize 2D rendering components
        if (!initialize2DComponents()) {
            std::cerr << "[SceneManager] Failed to initialize 2D components!" << std::endl;
            return false;
        }

        initialized = true;
        std::cout << "[SceneManager] Initialized with " << renderBackend->getBackendType()
            << " backend (" << width << "x" << height << ")" << std::endl;

        return true;
    }

    void SceneManager::shutdown() {
        if (!initialized) return;

        std::cout << "[SceneManager] Shutting down..." << std::endl;

        // Clear all scenes
        clearScenes();

        // Clear current transition
        currentTransition.reset();

        // Shutdown 2D components
        shutdown2DComponents();

        // Shutdown render backend
        if (renderBackend) {
            renderBackend->shutdown();
        }

        initialized = false;
        std::cout << "[SceneManager] Shutdown complete" << std::endl;
    }

    void SceneManager::pushScene(ScenePtr newScene, TransitionPtr transition) {
        if (!newScene) {
            std::cerr << "[SceneManager] Cannot push null scene!" << std::endl;
            return;
        }

        std::cout << "[SceneManager] Pushing scene: " << newScene->getName() << std::endl;

        // Deactivate current top scene if it pauses underlying scenes
        if (!sceneStack.empty()) {
            Scene* currentTop = sceneStack.back().get();
            if (currentTop && newScene->getPausesUnderlying()) {
                currentTop->onDeactivate();
            }
        }

        // Setup and add new scene
        setupScene(newScene.get());
        sceneStack.push_back(std::move(newScene));

        // Activate the new scene
        sceneStack.back()->onActivate();

        // Start transition if provided
        if (transition) {
            Scene* oldScene = (sceneStack.size() > 1) ? sceneStack[sceneStack.size() - 2].get() : nullptr;
            transition->begin(oldScene, sceneStack.back().get());
            currentTransition = std::move(transition);
        }
    }

    ScenePtr SceneManager::popScene(TransitionPtr transition) {
        if (sceneStack.empty()) {
            std::cerr << "[SceneManager] Cannot pop from empty scene stack!" << std::endl;
            return nullptr;
        }

        Scene* sceneToRemove = sceneStack.back().get();
        std::cout << "[SceneManager] Popping scene: " << sceneToRemove->getName() << std::endl;

        // Deactivate the scene being removed
        sceneToRemove->onDeactivate();

        // Start transition if provided
        Scene* newTop = (sceneStack.size() > 1) ? sceneStack[sceneStack.size() - 2].get() : nullptr;
        if (transition) {
            transition->begin(sceneToRemove, newTop);
            currentTransition = std::move(transition);
        }

        // Remove and cleanup scene
        ScenePtr removedScene = std::move(sceneStack.back());
        sceneStack.pop_back();
        cleanupScene(removedScene.get());

        // Activate new top scene if it was paused and no longer should be
        if (newTop && removedScene->getPausesUnderlying()) {
            newTop->onActivate();
        }

        return removedScene;
    }

    ScenePtr SceneManager::switchScene(ScenePtr newScene, TransitionPtr transition) {
        if (!newScene) {
            std::cerr << "[SceneManager] Cannot switch to null scene!" << std::endl;
            return nullptr;
        }

        std::cout << "[SceneManager] Switching to scene: " << newScene->getName() << std::endl;

        ScenePtr oldScene = nullptr;

        if (!sceneStack.empty()) {
            // Get current top scene
            Scene* currentTop = sceneStack.back().get();
            std::cout << "[SceneManager] Replacing scene: " << currentTop->getName() << std::endl;

            // Deactivate old scene
            currentTop->onDeactivate();

            // Start transition if provided
            if (transition) {
                transition->begin(currentTop, newScene.get());
                currentTransition = std::move(transition);
            }

            // Remove old scene
            oldScene = std::move(sceneStack.back());
            sceneStack.pop_back();
            cleanupScene(oldScene.get());
        } else {
            // No old scene, just start transition to new scene
            if (transition) {
                transition->begin(nullptr, newScene.get());
                currentTransition = std::move(transition);
            }
        }

        // Setup and add new scene
        setupScene(newScene.get());
        sceneStack.push_back(std::move(newScene));
        sceneStack.back()->onActivate();

        return oldScene;
    }

    void SceneManager::clearScenes() {
        std::cout << "[SceneManager] Clearing all scenes..." << std::endl;

        // Clear transition
        currentTransition.reset();

        // Deactivate and cleanup all scenes
        for (auto& scene : sceneStack) {
            if (scene) {
                scene->onDeactivate();
                cleanupScene(scene.get());
            }
        }

        sceneStack.clear();
        std::cout << "[SceneManager] All scenes cleared" << std::endl;
    }

    void SceneManager::update(float deltaTime) {
        if (!initialized) return;

        // Update transition first
        updateTransition(deltaTime);

        // Update resource manager loading jobs
        // This would advance asynchronous resource loading
        // resourceManager->updateLoadingJobs();

        // Update scenes (from bottom to top, but respect pause flags)
        for (auto& scene : sceneStack) {
            if (scene && !scene->isPaused()) {
                scene->update(deltaTime);

                // If this scene pauses underlying scenes, stop updating lower scenes
                if (scene->getPausesUnderlying()) {
                    // Only update this scene and higher
                    break;
                }
            }
        }
    }

    bool SceneManager::render() {
        if (!initialized || !renderBackend) {
            return false;
        }

        // Begin frame
        if (!renderBackend->beginFrame()) {
            return false;
        }

        // Clear render builder
        renderBuilder.clear();

        // Get camera and render target
        CameraParams camera = getCurrentCamera();
        RenderTarget target = getCurrentRenderTarget();

        // Render scenes
        for (auto& scene : sceneStack) {
            if (scene) {
                scene->render(renderBuilder);
            }
        }

        // Render transition overlay if active
        if (currentTransition) {
            currentTransition->renderOverlay(renderBuilder);
        }

        // Build command buffer and submit
        CommandBuffer commandBuffer = renderBuilder.flush(camera, target);

        // Update render stats
        lastRenderStats.totalDrawItems = commandBuffer.getDrawItems().size();
        lastRenderStats.totalPostEffects = commandBuffer.getPostEffects().size();
        lastRenderStats.totalUIItems = commandBuffer.getUIItems().size();
        lastRenderStats.commandBufferSize = commandBuffer.getTotalCommandCount();

        bool submitSuccess = renderBackend->submit(commandBuffer);

        // Present frame
        bool presentSuccess = renderBackend->present();

        return submitSuccess && presentSuccess;
    }

    Scene* SceneManager::getCurrentScene() const {
        return sceneStack.empty() ? nullptr : sceneStack.back().get();
    }

    Scene* SceneManager::getScene(size_t index) const {
        return (index < sceneStack.size()) ? sceneStack[index].get() : nullptr;
    }

    bool SceneManager::resize(uint32_t width, uint32_t height) {
        if (!initialized || !renderBackend) {
            return false;
        }

        renderWidth = width;
        renderHeight = height;

        // Update 2D camera viewport
        setup2DCamera();

        return renderBackend->resize(width, height);
    }

    std::string SceneManager::getDebugInfo() const {
        std::ostringstream oss;
        oss << "SceneManager Debug Info:\n";
        oss << "  Initialized: " << (initialized ? "Yes" : "No") << "\n";
        oss << "  Backend: " << (renderBackend ? renderBackend->getBackendType() : "None") << "\n";
        oss << "  Render Size: " << renderWidth << "x" << renderHeight << "\n";
        oss << "  Scene Count: " << sceneStack.size() << "\n";
        oss << "  Active Transition: " << (currentTransition ? currentTransition->getTransitionType() : "None") << "\n";

        if (!sceneStack.empty()) {
            oss << "  Scene Stack:\n";
            for (size_t i = 0; i < sceneStack.size(); ++i) {
                const auto& scene = sceneStack[i];
                oss << "    [" << i << "] " << scene->getName()
                    << " (WorldID: " << scene->getWorldId()
                    << ", Paused: " << (scene->isPaused() ? "Yes" : "No") << ")\n";
            }
        }

        return oss.str();
    }

    SceneManager::RenderStats SceneManager::getLastRenderStats() const {
        return lastRenderStats;
    }

    void SceneManager::setupScene(Scene* scene) {
        if (!scene) return;

        // Assign world ID
        scene->setWorldId(generateWorldId());

        // Call onAttach
        scene->onAttach(*this);

        std::cout << "[SceneManager] Scene '" << scene->getName()
            << "' attached with WorldID " << scene->getWorldId() << std::endl;
    }

    void SceneManager::cleanupScene(Scene* scene) {
        if (!scene) return;

        std::cout << "[SceneManager] Cleaning up scene: " << scene->getName() << std::endl;

        // Call onDetach
        scene->onDetach(*this);
    }

    void SceneManager::updateTransition(float deltaTime) {
        if (currentTransition) {
            bool isComplete = currentTransition->update(deltaTime);

            if (isComplete) {
                std::cout << "[SceneManager] Transition completed: "
                    << currentTransition->getTransitionType() << std::endl;
                currentTransition.reset();
            }
        }
    }

    CameraParams SceneManager::getCurrentCamera() const {
        // For now, return a default camera
        // In a real implementation, you'd get this from the current scene
        CameraParams camera;
        camera.position = math::Vec3f(0.0f, 0.0f, 5.0f);
        camera.forward = math::Vec3f(0.0f, 0.0f, -1.0f);
        camera.up = math::Vec3f(0.0f, 1.0f, 0.0f);

        // Calculate view matrix
        math::Vec3f target = camera.position + camera.forward;
        //camera.viewMatrix = math::lookAt(camera.position, target, camera.up);

        // Calculate projection matrix
        float aspect = static_cast<float>(renderWidth) / static_cast<float>(renderHeight);
        //camera.projectionMatrix = math::perspective(math::radians(camera.fov), aspect, camera.nearPlane, camera.farPlane);
        camera.projectionMatrix =
            math::Mat4::ortho(-400.f, 400.f,   // left, right
                -300.f, 300.f,   // bottom, top
                -1.f, 1.f);      // near,  far
        camera.viewMatrix = math::Mat4::identity();

        return camera;
    }

    RenderTarget SceneManager::getCurrentRenderTarget() const {
        RenderTarget target;
        target.width = renderWidth;
        target.height = renderHeight;
        target.targetId = "backbuffer";
        target.isBackbuffer = true;
        return target;
    }

    bool SceneManager::initialize2DComponents() {
        // Create Renderer2D
        renderer2D = createRenderer2D(*resourceManager, renderBuilder);

        RendererConfig2D config;
        config.maxQuadsPerBatch = 1000;
        config.enableBatching = true;
        config.enableSorting = true;

        if (!renderer2D->init(config)) {
            std::cerr << "[SceneManager] Failed to initialize Renderer2D!" << std::endl;
            return false;
        }

        // Create 2D facade
        facade2D = std::make_unique<Render2DFacade>();

        // Setup 2D camera
        setup2DCamera();

        std::cout << "[SceneManager] 2D components initialized successfully" << std::endl;
        return true;
    }

    void SceneManager::shutdown2DComponents() {
        facade2D.reset();

        if (renderer2D) {
            renderer2D->shutdown();
            renderer2D.reset();
        }

        std::cout << "[SceneManager] 2D components shutdown complete" << std::endl;
    }

    void SceneManager::setup2DCamera() {
        camera2D.setViewportSize(math::Vec2f(static_cast<float>(renderWidth), static_cast<float>(renderHeight)));
        camera2D.setPosition(math::Vec2f(0.0f, 0.0f));
        camera2D.setZoom(1.0f);
    }

    bool SceneManager::renderFrame() {
        if (!initialized || !renderBackend) {
            return false;
        }

        // Begin frame
        if (!renderBackend->beginFrame()) {
            return false;
        }

        // Clear render builder
        renderBuilder.clear();

        // Get camera and render target
        CameraParams camera = getCurrentCamera();
        RenderTarget target = getCurrentRenderTarget();

        // Render scenes
        for (auto& scene : sceneStack) {
            if (scene) {
                scene->render(renderBuilder);
            }
        }

        // Render transition overlay if active
        if (currentTransition) {
            currentTransition->renderOverlay(renderBuilder);
        }

        // Flush 2D facade if it has requests
        if (facade2D && renderer2D) {
            facade2D->flush(*renderer2D, camera2D);
        }

        // Build command buffer and submit
        CommandBuffer commandBuffer = renderBuilder.flush(camera, target);

        // Update render stats
        lastRenderStats.totalDrawItems = commandBuffer.getDrawItems().size();
        lastRenderStats.totalPostEffects = commandBuffer.getPostEffects().size();
        lastRenderStats.totalUIItems = commandBuffer.getUIItems().size();
        lastRenderStats.commandBufferSize = commandBuffer.getDrawItems().size() +
            commandBuffer.getPostEffects().size() +
            commandBuffer.getUIItems().size();

        // Add 2D stats if available
        if (renderer2D) {
            auto stats2D = renderer2D->getStats();
            lastRenderStats.quadCount = stats2D.quadCount;
            lastRenderStats.batchCount = stats2D.batchCount;
        }
        if (facade2D) {
            lastRenderStats.facadeLastFrameQuads = facade2D->getLastFrameQuadCount();
        }

        // Submit to backend
        if (!renderBackend->submit(commandBuffer)) {
            std::cerr << "[SceneManager] Failed to submit command buffer!" << std::endl;
            return false;
        }

        // Present
        if (!renderBackend->present()) {
            std::cerr << "[SceneManager] Failed to present frame!" << std::endl;
            return false;
        }

        return true;
    }

    void SceneManager::submit2DQuad(const math::Vec2f& position, const math::Vec2f& size,
        const Color& color, const std::string& texture,
        uint32_t layer, float depth) {
        if (facade2D) {
            if (texture.empty()) {
                facade2D->submitColoredQuad(position, size, color, layer, depth);
            } else {
                facade2D->submitSprite(position, size, texture, color, layer, depth);
            }
        }
    }

    void SceneManager::clear2DRequests() {
        if (facade2D) {
            facade2D->clear();
        }
    }

    void SceneManager::printStats() const {
        if (!renderer2D || !facade2D) {
            return;
        }

        auto stats = renderer2D->getStats();
        auto queueSizes = renderBuilder.getQueueSizes();

        std::cout << "[SceneManager Stats] - Scenes: " << sceneStack.size()
            << ", Quads: " << stats.quadCount
            << ", Batches: " << stats.batchCount
            << ", Queue Draw Items: " << queueSizes.drawItems
            << ", Facade Last Frame: " << facade2D->getLastFrameQuadCount() << std::endl;
    }

} // namespace scene

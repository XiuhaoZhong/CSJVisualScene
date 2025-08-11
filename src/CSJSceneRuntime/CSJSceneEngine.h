#ifndef __CSJSCENEENGINE_H__
#define __CSJSCENEENGINE_H__

#include <chrono>
#include <memory>

#include <QVulkanWindow>
#include <QVulkanDeviceFunctions>
#include <vulkan/vulkan_core.h>

class CSJLogger;

namespace CSJEngine {

class CSJRuntimeContext;

class CSJSceneEngine : public QVulkanWindowRenderer {
public:
    CSJSceneEngine(QVulkanWindow *window);
    ~CSJSceneEngine();

    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;

    void startNextFrame() override;

protected:
    float calculateDeltaTime();

private:
    CSJLogger         *m_pLogger;
    QVulkanWindow     *m_pWindow;
    CSJRuntimeContext *m_pGlobalContext;
    std::chrono::steady_clock::time_point m_last_tick_time;
};

using CSJSpGLRenderWidget = std::shared_ptr<CSJSceneEngine>;

} // namespace CSJEngine

#endif // __CSJSCENEENGINE_H__
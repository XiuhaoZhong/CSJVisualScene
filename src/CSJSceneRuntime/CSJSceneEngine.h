#ifndef __CSJSCENEENGINE_H__
#define __CSJSCENEENGINE_H__

#include <memory>

#include <QVulkanWindow>
#include <QVulkanDeviceFunctions>

#include "CSJGLRender/CSJGLSceneBase.h"

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
    
private:
    QVulkanWindow *m_pWindow;
    QVulkanDeviceFunctions *m_pFunctions;
};

using CSJSpGLRenderWidget = std::shared_ptr<CSJSceneEngine>;

#endif // __CSJSCENEENGINE_H__
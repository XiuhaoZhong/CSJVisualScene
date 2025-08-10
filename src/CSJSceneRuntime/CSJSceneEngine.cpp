#include "CSJSceneEngine.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Utils/CSJLogger.h"
#include "CSJSceneRuntime/core/math/coordinate_definitions.h"
#include "CSJSceneRuntime/function/global/CSJRuntimeContext.h"

namespace CSJEngine {

CSJSceneEngine::CSJSceneEngine(QVulkanWindow* window)
    : m_pWindow(window)
    , m_pLogger(CSJLogger::getLoggerInst())
    , m_pGlobalContext(CSJRuntimeContext::getRuntimeContextInstance()) {
}

CSJSceneEngine::~CSJSceneEngine() {

}

void CSJSceneEngine::initResources() {
    m_pLogger->log_info("initResources!");

    bool ret = m_pGlobalContext->initialize();
    if (!ret) {
        m_pLogger->log_error("CSJRuntimeContext initialize failed!");
    } else {
        m_pLogger->log_error("CSJRuntimeContext initialize successfuly!");
    }
}

void CSJSceneEngine::initSwapChainResources() {
    m_pLogger->log_info("initSwapChainReousrces!");

    if (!m_pGlobalContext->getRenderSystem()->isInit()) {
        CSJRenderSystemContext render_system_ctx = {
        {m_pWindow->swapChainImageCount(),m_pWindow->swapChainImageSize().width(), m_pWindow->swapChainImageSize().height()},
        m_pWindow->graphicsQueue(),
        m_pWindow->graphicsCommandPool(),
        m_pWindow->currentCommandBuffer(),
        m_pWindow->colorFormat(),
        m_pWindow->device(),
        m_pWindow->physicalDevice()
        };

        m_pGlobalContext->getRenderSystem()->updateRenderSystemContext(render_system_ctx);
        m_pGlobalContext->getRenderSystem()->initialize();
    }

    CSJSwapChainInfo swapSize = {m_pWindow->swapChainImageCount(), 
                                 m_pWindow->swapChainImageSize().width(), 
                                 m_pWindow->swapChainImageSize().height()};

    m_pGlobalContext->getRenderSystem()->updateSwapChainInfo(swapSize);

    int swapChainImageCount = m_pWindow->swapChainImageCount();
    std::vector<VkImageView> swapFrameBuffers(swapChainImageCount);

    for (size_t i = 0; i < swapChainImageCount; i++) {
        swapFrameBuffers[i] = m_pWindow->swapChainImageView(i);
    }
    m_pGlobalContext->getRenderSystem()->loadDepthAndFrameBuffers(swapFrameBuffers);
}

void CSJSceneEngine::releaseSwapChainResources() {
    m_pLogger->log_info("releaseSwapChainResources!");

    m_pGlobalContext->getRenderSystem()->releaseDepthAndFrameBuffers();
}

void CSJSceneEngine::releaseResources() {
    m_pLogger->log_info("releaseResources!");
}

void CSJSceneEngine::startNextFrame() {
    /*************************************************/
    /* tick circle                                   */
    /* logic tick                                    */
    /* render tick                                   */
    /*************************************************/
    m_pGlobalContext->getRenderSystem()->render_tick(m_pWindow->currentCommandBuffer(), 
                                                     0.0, 
                                                     m_pWindow->currentSwapChainImageIndex());

    m_pWindow->frameReady();
    m_pWindow->requestUpdate();
}

} // namespace CSJEngine
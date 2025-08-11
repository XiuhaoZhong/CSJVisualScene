#include "CSJSceneEngine.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Utils/CSJLogger.h"
#include "CSJSceneRuntime/core/math/coordinate_definitions.h"
#include "CSJSceneRuntime/function/global/CSJRuntimeContext.h"
#include "CSJSceneRuntime/function/render/render_system.h"

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
    float delta_time = calculateDeltaTime();
    m_pGlobalContext->getRenderSystem()->render_tick(m_pWindow->currentCommandBuffer(), 
                                                     delta_time, 
                                                     m_pWindow->currentSwapChainImageIndex());

    m_pWindow->frameReady();
    m_pWindow->requestUpdate();
}

float CSJSceneEngine::calculateDeltaTime() {
    float delta_time = 0.0;
    {
        using namespace std::chrono;
        steady_clock::time_point current_tick_time = steady_clock::now();
        duration<float> time_span = duration_cast<duration<float>>(current_tick_time - m_last_tick_time);
        delta_time = time_span.count();

        m_last_tick_time = current_tick_time;
    }

    return delta_time;
}

} // namespace CSJEngine
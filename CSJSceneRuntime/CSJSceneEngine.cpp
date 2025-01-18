#include "CSJSceneEngine.h"

#include <QDebug>

CSJSceneEngine::CSJSceneEngine(QVulkanWindow* window)
    : m_pWindow(window) {
    
}

CSJSceneEngine::~CSJSceneEngine() {

}

void CSJSceneEngine::initResources() {
    qDebug() << "initResources!";

    VkDevice device = m_pWindow->device();
    m_pFunctions = m_pWindow->vulkanInstance()->deviceFunctions(device);

    const int frameInFlight = m_pWindow->concurrentFrameCount();
}

void CSJSceneEngine::initSwapChainResources() {
    qDebug() << "initSwapChainReousrces!";
}

void CSJSceneEngine::releaseSwapChainResources() {
    qDebug() << "releaseSwapChainResources!";
}

void CSJSceneEngine::releaseResources() {
    qDebug() << "releaseResources!";
}

void CSJSceneEngine::startNextFrame() {
    VkDevice device = m_pWindow->device();
    VkCommandBuffer commandBuffer = m_pWindow->currentCommandBuffer();
    const QSize frameSize = m_pWindow->swapChainImageSize();

    VkClearColorValue clearColor = {1.0f, 0.5f, 0.5f, 1.0f};
    VkClearDepthStencilValue clearDS = {1, 0};

    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo passBeginInfo{};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.renderPass = m_pWindow->defaultRenderPass();
    passBeginInfo.framebuffer = m_pWindow->currentFramebuffer();
    passBeginInfo.renderArea.extent.width = frameSize.width();
    passBeginInfo.renderArea.extent.height = frameSize.height();
    passBeginInfo.clearValueCount = 2;
    passBeginInfo.pClearValues = clearValues;
    VkCommandBuffer cmdBuffer = m_pWindow->currentCommandBuffer();
    m_pFunctions->vkCmdBeginRenderPass(cmdBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewPort{};
    viewPort.x = viewPort.y = 0;
    viewPort.width = frameSize.width();
    viewPort.height = frameSize.height();
    viewPort.minDepth = 0;
    viewPort.maxDepth = 1;
    m_pFunctions->vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewPort.width;
    scissor.extent.height = viewPort.height;
    m_pFunctions->vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    m_pFunctions->vkCmdEndRenderPass(commandBuffer);

    m_pWindow->frameReady();
    m_pWindow->requestUpdate();
}

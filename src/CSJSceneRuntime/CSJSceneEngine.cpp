#include "CSJSceneEngine.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <stdexcept>
#include <vulkan/vulkan_core.h>

// #include "stbi/stb_image.h"
// #include "tinyobjloader/tiny_obj_loader.h"

#include "Utils/CSJLogger.h"

#include "CSJSceneRuntime/core/math/coordinate_definitions.h"
#include "CSJSceneRuntime/function/global/CSJRuntimeContext.h"

using namespace CSJEngine;

static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

const std::string MODEL_PATH = "resources/models/viking_room.obj";
const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0,1,2,2,3,0,
    4,5,6,6,7,4
};

std::vector<Vertex> obj_verteices;
std::vector<uint32_t> obj_indices;

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
    m_pGlobalContext->getRenderSystem()->render_tick(m_pWindow->currentCommandBuffer(), 0.0, m_pWindow->currentSwapChainImageIndex());

    m_pWindow->frameReady();
    m_pWindow->requestUpdate();
}

#include "CSJSceneEngine.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <QDebug>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "glm/fwd.hpp"
#include "glm/gtx/hash.hpp"

#include "stbi/stb_image.h"
#include "tinyobjloader/tiny_obj_loader.h"

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
    : m_pWindow(window) {
    
        m_isInit = false;
}

CSJSceneEngine::~CSJSceneEngine() {

}

void CSJSceneEngine::initResources() {
    qDebug() << "initResources!";

    VkDevice device = m_pWindow->device();
    m_pFunctions = m_pWindow->vulkanInstance()->deviceFunctions(device);
    m_pVulkanFunctions = m_pWindow->vulkanInstance()->functions();

    std::vector<tinyobj::shape_t> shapes;
    if (shapes.size() > 0) {
        std::cout << "There are shapes" << std::endl;
    } else {
        std::cout << "There aren't any shapes" << std::endl;
    }
}

void CSJSceneEngine::initSwapChainResources() {
    qDebug() << "initSwapChainReousrces!";
    m_max_frames_in_flight = m_pWindow->swapChainImageCount();

    if (!m_isInit) {
        loadModels();
        createImageResources();
        m_isInit = true;
    }

    createDepthResources();
    createFramebuffers();
}

void CSJSceneEngine::releaseSwapChainResources() {
    qDebug() << "releaseSwapChainResources!";

    releaseFramebuffers();
    releaseDepthResources();
}

void CSJSceneEngine::releaseResources() {
    qDebug() << "releaseResources!";
}

std::vector<char> CSJSceneEngine::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule CSJSceneEngine::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (m_pFunctions->vkCreateShaderModule(m_pWindow->device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }

    return shaderModule;
}

void CSJSceneEngine::createImageUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_image_uniform_buffers.resize(m_max_frames_in_flight);
    m_image_uniform_buffer_memories.resize(m_max_frames_in_flight);
    m_image_uniform_buffer_mappeds.resize(m_max_frames_in_flight);

    for (size_t i = 0 ; i < m_max_frames_in_flight; i++) {
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     m_image_uniform_buffers[i],
                     m_image_uniform_buffer_memories[i]);
        
        m_pFunctions->vkMapMemory(m_pWindow->device(), m_image_uniform_buffer_memories[i], 0, bufferSize, 0, &m_image_uniform_buffer_mappeds[i]);
    }
}

void CSJSceneEngine::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(m_max_frames_in_flight);
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(m_max_frames_in_flight);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = static_cast<uint32_t>(m_max_frames_in_flight);

    if (m_pFunctions->vkCreateDescriptorPool(m_pWindow->device(), &poolInfo, nullptr, &m_image_descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create diecriptor pool!");
    }
}

void CSJSceneEngine::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding            = 0;
    uboLayoutBinding.descriptorCount    = 1;
    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding            = 1;
    samplerLayoutBinding.descriptorCount    = 1;
    samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings    = bindings.data();

    if (m_pFunctions->vkCreateDescriptorSetLayout(m_pWindow->device(), &layoutInfo, nullptr, &m_image_descriptorset_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout");
    }
}

void CSJSceneEngine::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(m_max_frames_in_flight, m_image_descriptorset_layout);
    VkDescriptorSetAllocateInfo allocInfo{};

    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = m_image_descriptor_pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_max_frames_in_flight);
    allocInfo.pSetLayouts        = layouts.data();

    m_image_descriptor_sets.resize(m_max_frames_in_flight);
    if (m_pFunctions->vkAllocateDescriptorSets(m_pWindow->device(), &allocInfo, m_image_descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < m_max_frames_in_flight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_image_uniform_buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = m_image_texture_imageview;
        imageInfo.sampler     = m_image_texture_sampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet          = m_image_descriptor_sets[i];
        descriptorWrites[0].dstBinding      = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo     = &bufferInfo;

        descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet          = m_image_descriptor_sets[i];
        descriptorWrites[1].dstBinding      = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo      = &imageInfo;

        m_pFunctions->vkUpdateDescriptorSets(m_pWindow->device(), 
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0, nullptr);
    }
}

void CSJSceneEngine::createImageRenderPipeline() {
    auto vertShaderCode = readFile("resources/shaders/vert.spv");
    auto fragShaderCode = readFile("resources/shaders/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription    = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable       = VK_TRUE;
    depthStencilInfo.depthWriteEnable      = VK_TRUE;
    depthStencilInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.stencilTestEnable     = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                          VK_COLOR_COMPONENT_G_BIT | 
                                          VK_COLOR_COMPONENT_B_BIT | 
                                          VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts    = &m_image_descriptorset_layout;
    //pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (m_pFunctions->vkCreatePipelineLayout(m_pWindow->device(), &pipelineLayoutInfo, 
        nullptr, &m_image_pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencilInfo;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = m_image_pipeline_layout;
    pipelineInfo.renderPass          = m_main_render_pass;
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    if (m_pFunctions->vkCreateGraphicsPipelines(m_pWindow->device(), VK_NULL_HANDLE, 1, 
                                  &pipelineInfo, nullptr, 
                                  &m_image_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    m_pFunctions->vkDestroyShaderModule(m_pWindow->device(), fragShaderModule, nullptr);
    m_pFunctions->vkDestroyShaderModule(m_pWindow->device(), vertShaderModule, nullptr);
}

void CSJSceneEngine::createFramebuffers() {
    int swapChainSize = m_pWindow->swapChainImageCount();
    m_framebuffers.resize(swapChainSize);

    for (size_t i = 0; i < swapChainSize; i++) {
        std::array<VkImageView, 2> attachments = {
            m_pWindow->swapChainImageView(i),
            m_depth_image_view
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_main_render_pass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_pWindow->swapChainImageSize().width();
        framebufferInfo.height = m_pWindow->swapChainImageSize().height();
        framebufferInfo.layers = 1;

        if (m_pFunctions->vkCreateFramebuffer(m_pWindow->device(), &framebufferInfo, nullptr, 
                                &m_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create frame buffer!");
        }
    }
}

void CSJSceneEngine::releaseFramebuffers() {
    if (m_framebuffers.size() == 0) {
        return ;
    }

    for (auto framebuffer : m_framebuffers) {
        m_pFunctions->vkDestroyFramebuffer(m_pWindow->device(), framebuffer, nullptr);
    }
}

VkFormat CSJSceneEngine::findSupportedFormat(const std::vector<VkFormat>& candidates, 
                                             VkImageTiling tiling, 
                                             VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        m_pVulkanFunctions->vkGetPhysicalDeviceFormatProperties(m_pWindow->physicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat CSJSceneEngine::findDepthFormat() {
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT, 
        VK_FORMAT_D32_SFLOAT_S8_UINT, 
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    return findSupportedFormat(candidates, 
                               VK_IMAGE_TILING_OPTIMAL, 
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void CSJSceneEngine::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    QSize extentSize = m_pWindow->swapChainImageSize();

    createImage(extentSize.width(), extentSize.height(), 1, depthFormat, 
                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_image, m_depth_image_memory);
    m_depth_image_view = createImageView(m_depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

void CSJSceneEngine::releaseDepthResources() {
    VkDevice device = m_pWindow->device();
    m_pFunctions->vkDestroyImage(device, m_depth_image, nullptr);
    m_pFunctions->vkFreeMemory(device, m_depth_image_memory, nullptr);
    m_pFunctions->vkDestroyImageView(device, m_depth_image_view, nullptr);
}

void CSJSceneEngine::createMainRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format          = m_pWindow->colorFormat();
    colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format         = findDepthFormat();
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    if (m_pFunctions->vkCreateRenderPass(m_pWindow->device(), &renderPassInfo, nullptr, &m_main_render_pass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void CSJSceneEngine::drawImage(VkCommandBuffer commandBuffer, int index) {
    updateUniformBuffer(index);

    const QSize frameSize = m_pWindow->swapChainImageSize();

    VkClearColorValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    VkClearDepthStencilValue clearDS = {1, 0};

    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo passBeginInfo{};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.renderPass = m_main_render_pass;
    passBeginInfo.framebuffer = m_framebuffers[m_pWindow->currentSwapChainImageIndex()];
    passBeginInfo.renderArea.extent.width = frameSize.width();
    passBeginInfo.renderArea.extent.height = frameSize.height();
    passBeginInfo.clearValueCount = 2;
    passBeginInfo.pClearValues = clearValues;
    VkCommandBuffer cmdBuffer = m_pWindow->currentCommandBuffer();
    m_pFunctions->vkCmdBeginRenderPass(cmdBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    m_pFunctions->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_image_pipeline);

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

    VkBuffer vertexBuffers[] = {
        m_vertex_buffer
    };
    VkDeviceSize offsets[] = {0};
    m_pFunctions->vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    m_pFunctions->vkCmdBindIndexBuffer(commandBuffer, m_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    m_pFunctions->vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
                                          m_image_pipeline_layout, 0, 1, &m_image_descriptor_sets[index], 0, nullptr);
    
    //m_pFunctions->vkCmdDraw(commandBuffer, static_cast<uint32_t>(obj_verteices.size()), 1, 0, 0);
    m_pFunctions->vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(obj_indices.size()), 1, 0, 0, 0);

    m_pFunctions->vkCmdEndRenderPass(commandBuffer);
}

void CSJSceneEngine::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model    = glm::rotate(glm::mat4(1.0f), 0.0f/*time * glm::radians(90.0f)*/, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view     = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    float aspect = (float) m_pWindow->swapChainImageSize().width() / (float) m_pWindow->swapChainImageSize().height();
    ubo.proj     = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_image_uniform_buffer_mappeds[currentImage], &ubo, sizeof(ubo));
}

void CSJSceneEngine::createVertexBuffer() {
    std::vector<Vertex> current_vertices = obj_verteices;//vertices;
    VkDeviceSize bufferSize = sizeof(current_vertices[0]) * current_vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void *data;
    m_pFunctions->vkMapMemory(m_pWindow->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, current_vertices.data(), (size_t)bufferSize);
    m_pFunctions->vkUnmapMemory(m_pWindow->device(), stagingBufferMemory);

    createBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_vertex_buffer,
                 m_vertex_buffer_memory);
    
    copyBuffer(stagingBuffer, m_vertex_buffer, bufferSize);

    m_pFunctions->vkDestroyBuffer(m_pWindow->device(), stagingBuffer, nullptr);
    m_pFunctions->vkFreeMemory(m_pWindow->device(), stagingBufferMemory, nullptr);
}

void CSJSceneEngine::createIndexBuffer() {
    std::vector<uint32_t> currentIndices = obj_indices;

    VkDeviceSize bufferSize = sizeof(currentIndices[0]) * currentIndices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    m_pFunctions->vkMapMemory(m_pWindow->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, currentIndices.data(), (size_t)bufferSize);
    m_pFunctions->vkUnmapMemory(m_pWindow->device(), stagingBufferMemory);

    createBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_index_buffer, m_index_buffer_memory);

    copyBuffer(stagingBuffer, m_index_buffer, bufferSize);

    m_pFunctions->vkDestroyBuffer(m_pWindow->device(), stagingBuffer, nullptr);
    m_pFunctions->vkFreeMemory(m_pWindow->device(), stagingBufferMemory, nullptr);
}

void CSJSceneEngine::createTextureImage() {
    // resources/originImages/slamDumk_images/cross_street.jpeg
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4;
    m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data;
    m_pFunctions->vkMapMemory(m_pWindow->device(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    m_pFunctions->vkUnmapMemory(m_pWindow->device(), stagingBufferMemory);
    stbi_image_free(pixels);

    createImage(texWidth, texHeight, m_mipLevels, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                m_image_texture_image, m_image_texture_image_memory);

    transitionImageLayout(m_image_texture_image, VK_FORMAT_R8G8B8A8_SRGB, 
                          VK_IMAGE_LAYOUT_UNDEFINED, 
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
    copyBufferToImage(stagingBuffer, m_image_texture_image, 
                      static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(m_image_texture_image, VK_FORMAT_R8G8B8A8_SRGB, 
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipLevels);

    m_pFunctions->vkDestroyBuffer(m_pWindow->device(), stagingBuffer, nullptr);
    m_pFunctions->vkFreeMemory(m_pWindow->device(), stagingBufferMemory, nullptr);

    generateMipmaps(m_image_texture_image, VK_FORMAT_R8G8_SRGB, texWidth, texHeight, m_mipLevels);
}

void CSJSceneEngine::loadModels() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVerteices{};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                (1.0f - attrib.texcoords[2 * index.texcoord_index + 1])
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVerteices.count(vertex) == 0) {
                uniqueVerteices[vertex] = static_cast<uint32_t>(obj_verteices.size());
                obj_verteices.push_back(vertex);
            }

            obj_indices.push_back(uniqueVerteices[vertex]);
            //obj_verteices.push_back(vertex);
            //obj_indices.push_back(indices.size());
        }
    }

    // without index, the vertex total number is 11484!
    std::cout << "*******************************************" << std::endl;
    std::cout << "Model loading compeleted, total vertex number: " << obj_verteices.size() << std::endl;
    std::cout << "*******************************************" << std::endl;
}

void CSJSceneEngine::createImageResources() {
    createMainRenderPass();
    createDescriptorSetLayout();
    createImageRenderPipeline();
    createTextureImage();
    createImageTextureImageView();
    createImageTextureSampler();
    createImageUniformBuffers();
    createVertexBuffer();
    createIndexBuffer();
    createDescriptorPool();
    createDescriptorSets();
}

void CSJSceneEngine::releaseImageResources() {
    VkDevice device = m_pWindow->device();
    m_pFunctions->vkDestroyDescriptorSetLayout(device, m_image_descriptorset_layout, nullptr);

    m_pFunctions->vkDestroyDescriptorPool(device, m_image_descriptor_pool, nullptr);
    m_pFunctions->vkDestroyBuffer(device, m_index_buffer, nullptr);
    m_pFunctions->vkFreeMemory(device, m_index_buffer_memory, nullptr);
    m_pFunctions->vkDestroyBuffer(device, m_vertex_buffer, nullptr);
    m_pFunctions->vkFreeMemory(device, m_vertex_buffer_memory, nullptr);
    
    for (size_t i = 0; i < m_max_frames_in_flight; i++) {
        m_pFunctions->vkDestroyBuffer(device, m_image_uniform_buffers[i], nullptr);
        m_pFunctions->vkFreeMemory(device, m_image_uniform_buffer_memories[i], nullptr);
    }

    m_pFunctions->vkDestroySampler(device, m_image_texture_sampler, nullptr);

    m_pFunctions->vkDestroyImageView(device, m_image_texture_imageview, nullptr);
    m_pFunctions->vkDestroyImage(device, m_image_texture_image, nullptr);
    m_pFunctions->vkDestroyPipeline(device, m_image_pipeline, nullptr);
    m_pFunctions->vkDestroyPipelineLayout(device, m_image_pipeline_layout, nullptr);
    m_pFunctions->vkDestroyRenderPass(device, m_main_render_pass, nullptr);
}

void CSJSceneEngine::startNextFrame() {
    VkDevice device = m_pWindow->device();
    VkCommandBuffer commandBuffer = m_pWindow->currentCommandBuffer();

    drawImage(commandBuffer, m_pWindow->currentSwapChainImageIndex());

    m_pWindow->frameReady();
    m_pWindow->requestUpdate();
}

uint32_t CSJSceneEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    m_pVulkanFunctions->vkGetPhysicalDeviceMemoryProperties(m_pWindow->physicalDevice(), &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void CSJSceneEngine::createBuffer(VkDeviceSize size, 
                                  VkBufferUsageFlags usage, 
                                  VkMemoryPropertyFlags properties, 
                                  VkBuffer &buffer, 
                                  VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_pFunctions->vkCreateBuffer(m_pWindow->device(), &createInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    m_pFunctions->vkGetBufferMemoryRequirements(m_pWindow->device(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (m_pFunctions->vkAllocateMemory(m_pWindow->device(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    m_pFunctions->vkBindBufferMemory(m_pWindow->device(), buffer, bufferMemory, 0);
}

void CSJSceneEngine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_pWindow->graphicsCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    m_pFunctions->vkAllocateCommandBuffers(m_pWindow->device(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    m_pFunctions->vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    m_pFunctions->vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    m_pFunctions->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    m_pFunctions->vkQueueSubmit(m_pWindow->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    m_pFunctions->vkQueueWaitIdle(m_pWindow->graphicsQueue());

    m_pFunctions->vkFreeCommandBuffers(m_pWindow->device(), m_pWindow->graphicsCommandPool(), 1, &commandBuffer);
}

void CSJSceneEngine::copyBufferToImage(VkBuffer buffer, VkImage image,
                                       uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    m_pFunctions->vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void CSJSceneEngine::transitionImageLayout(VkImage image, VkFormat format,
                                           VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    m_pFunctions->vkCmdPipelineBarrier(commandBuffer,
                                       sourceStage, 
                                       destinationStage,
                                       0,0, nullptr,0,
                                       nullptr,1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void CSJSceneEngine::generateMipmaps(VkImage image, VkFormat format, 
                                     int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    
    VkFormatProperties formatProperties;
    m_pVulkanFunctions->vkGetPhysicalDeviceFormatProperties(m_pWindow->physicalDevice(), format, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting");
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image                           = image;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

        m_pFunctions->vkCmdPipelineBarrier(commandBuffer, 
                                           VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                           VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 
                                           0, nullptr, 
                                           0, nullptr, 
                                           1, &barrier);
        
        VkImageBlit blit{};
        blit.srcOffsets[0]                 = {0, 0, 0};
        blit.srcOffsets[1]                 = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel       = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount     = 1;
        blit.dstOffsets[0]                 = {0, 0, 0};
        blit.dstOffsets[1]                 = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel       = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount     = 1;

        m_pFunctions->vkCmdBlitImage(commandBuffer, 
                                     image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
                                     image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                     1, &blit, VK_FILTER_LINEAR);

        if (mipWidth > 1) {
            mipWidth /= 2;
        }

        if (mipHeight > 1) {
            mipHeight /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

    m_pFunctions->vkCmdPipelineBarrier(commandBuffer,
                                       VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
                                       0,
                                       0, nullptr,
                                       0, nullptr,
                                       1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer CSJSceneEngine::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_pWindow->graphicsCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    m_pFunctions->vkAllocateCommandBuffers(m_pWindow->device(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    m_pFunctions->vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer; 
}

void CSJSceneEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    m_pFunctions->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    m_pFunctions->vkQueueSubmit(m_pWindow->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    m_pFunctions->vkQueueWaitIdle(m_pWindow->graphicsQueue());

    m_pFunctions->vkFreeCommandBuffers(m_pWindow->device(), m_pWindow->graphicsCommandPool(), 1, &commandBuffer);
}

void CSJSceneEngine::createImageTextureSampler() {
    VkPhysicalDeviceProperties properties{};
    m_pVulkanFunctions->vkGetPhysicalDeviceProperties(m_pWindow->physicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = VK_FILTER_LINEAR;
    samplerInfo.minFilter               = VK_FILTER_LINEAR;
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    samplerInfo.minLod                  = 0;
    samplerInfo.maxLod                  = VK_LOD_CLAMP_NONE;
    samplerInfo.mipLodBias              = 0.0f;

    if (m_pFunctions->vkCreateSampler(m_pWindow->device(), &samplerInfo, nullptr, &m_image_texture_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void CSJSceneEngine::createImageTextureImageView() {
    m_image_texture_imageview = createImageView(m_image_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
}

void CSJSceneEngine::createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                                 VkFormat format, VkImageTiling tiling, 
                                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                                 VkImage &image, VkDeviceMemory &imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = mipLevels;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (m_pFunctions->vkCreateImage(m_pWindow->device(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    m_pFunctions->vkGetImageMemoryRequirements(m_pWindow->device(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (m_pFunctions->vkAllocateMemory(m_pWindow->device(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    m_pFunctions->vkBindImageMemory(m_pWindow->device(), image, imageMemory, 0);

}

VkImageView CSJSceneEngine::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (m_pFunctions->vkCreateImageView(m_pWindow->device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    return imageView;
}

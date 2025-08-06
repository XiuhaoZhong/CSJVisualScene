#include "render_system.h"

#include <fstream>

#include <vulkan/vulkan.h>

#include "stbi/stb_image.h"
#include "tinyobjloader/tiny_obj_loader.h"

#include "CSJSceneRuntime/core/math/coordinate_definitions.h"
#include "Utils/CSJLogger.h"

static std::string MODEL_PATH = "resources/models/viking_room.obj";
static std::string TEXTURE_PATH = "resources/textures/viking_room.png";

struct CSJUniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct CSJVertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(CSJVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(CSJVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(CSJVertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(CSJVertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const CSJVertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<CSJVertex> {
        size_t operator()(CSJVertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

const std::vector<CSJVertex> vertices = {
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

static std::vector<CSJVertex> obj_verteices;
static std::vector<uint32_t> obj_indices;

namespace CSJEngine {
CSJRenderSystem::CSJRenderSystem()
    : m_init(false) {

}

CSJRenderSystem::~CSJRenderSystem() {
    m_pLogger = CSJLogger::getLoggerInst();
}

bool CSJRenderSystem::isInit() {
    return m_init;
}

bool CSJRenderSystem::initialize() {
    if (m_init) {
        return true;
    }

    m_init = true;
    loadModels();
    createImageResources();

    return m_init;
}

void CSJRenderSystem::render_tick(VkCommandBuffer commandBuffer, double timestamp, int swapFrameIndex) {
    drawImage(commandBuffer, timestamp, swapFrameIndex);
}

std::vector<char> CSJRenderSystem::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        m_pLogger->log_warn("Can't open file {}", filename);
        return std::vector<char>();
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule CSJRenderSystem::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_rendererContext.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }

    return shaderModule;
}

void CSJRenderSystem::createImageUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(CSJUniformBufferObject);

    m_image_uniform_buffers.resize(m_rendererContext.swapChainInfo.max_frames_in_flight);
    m_image_uniform_buffer_memories.resize(m_rendererContext.swapChainInfo.max_frames_in_flight);
    m_image_uniform_buffer_mappeds.resize(m_rendererContext.swapChainInfo.max_frames_in_flight);

    for (size_t i = 0 ; i < m_rendererContext.swapChainInfo.max_frames_in_flight; i++) {
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     m_image_uniform_buffers[i],
                     m_image_uniform_buffer_memories[i]);
        
        vkMapMemory(m_rendererContext.device, m_image_uniform_buffer_memories[i], 0, bufferSize, 0, &m_image_uniform_buffer_mappeds[i]);
    }
}

void CSJRenderSystem::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(m_rendererContext.swapChainInfo.max_frames_in_flight);
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(m_rendererContext.swapChainInfo.max_frames_in_flight);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = static_cast<uint32_t>(m_rendererContext.swapChainInfo.max_frames_in_flight);

    if (vkCreateDescriptorPool(m_rendererContext.device, &poolInfo, nullptr, &m_image_descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create diecriptor pool!");
    }
}

void CSJRenderSystem::createDescriptorSetLayout() {
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

    if (vkCreateDescriptorSetLayout(m_rendererContext.device, &layoutInfo, nullptr, &m_image_descriptorset_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout");
    }
}

void CSJRenderSystem::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(m_rendererContext.swapChainInfo.max_frames_in_flight, m_image_descriptorset_layout);
    VkDescriptorSetAllocateInfo allocInfo{};

    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = m_image_descriptor_pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_rendererContext.swapChainInfo.max_frames_in_flight);
    allocInfo.pSetLayouts        = layouts.data();

    m_image_descriptor_sets.resize(m_rendererContext.swapChainInfo.max_frames_in_flight);
    if (vkAllocateDescriptorSets(m_rendererContext.device, &allocInfo, m_image_descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < m_rendererContext.swapChainInfo.max_frames_in_flight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_image_uniform_buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(CSJUniformBufferObject);

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

        vkUpdateDescriptorSets(m_rendererContext.device, 
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0, nullptr);
    }
}

void CSJRenderSystem::createImageRenderPipeline() {
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

    auto bindingDescription    = CSJVertex::getBindingDescription();
    auto attributeDescriptions = CSJVertex::getAttributeDescription();

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

    if (vkCreatePipelineLayout(m_rendererContext.device, &pipelineLayoutInfo, nullptr, &m_image_pipeline_layout) != VK_SUCCESS) {
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

    if (vkCreateGraphicsPipelines(m_rendererContext.device, VK_NULL_HANDLE, 1, 
                                  &pipelineInfo, nullptr, 
                                  &m_image_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_rendererContext.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_rendererContext.device, vertShaderModule, nullptr);
}

void CSJRenderSystem::createImageTextureImageView() {
    m_image_texture_imageview = createImageView(m_image_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
}

void CSJRenderSystem::createImageTextureSampler() {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_rendererContext.physicalDevice, &properties);

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

    if (vkCreateSampler(m_rendererContext.device, &samplerInfo, nullptr, &m_image_texture_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void CSJRenderSystem::createImageResources() {
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

void CSJRenderSystem::releaseImageResources() {
    vkDestroyDescriptorSetLayout(m_rendererContext.device, m_image_descriptorset_layout, nullptr);

    vkDestroyDescriptorPool(m_rendererContext.device, m_image_descriptor_pool, nullptr);
    vkDestroyBuffer(m_rendererContext.device, m_index_buffer, nullptr);
    vkFreeMemory(m_rendererContext.device, m_index_buffer_memory, nullptr);
    vkDestroyBuffer(m_rendererContext.device, m_vertex_buffer, nullptr);
    vkFreeMemory(m_rendererContext.device, m_vertex_buffer_memory, nullptr);
    
    for (size_t i = 0; i < m_rendererContext.swapChainInfo.max_frames_in_flight; i++) {
        vkDestroyBuffer(m_rendererContext.device, m_image_uniform_buffers[i], nullptr);
        vkFreeMemory(m_rendererContext.device, m_image_uniform_buffer_memories[i], nullptr);
    }

    vkDestroySampler(m_rendererContext.device, m_image_texture_sampler, nullptr);

    vkDestroyImageView(m_rendererContext.device, m_image_texture_imageview, nullptr);
    vkDestroyImage(m_rendererContext.device, m_image_texture_image, nullptr);
    vkDestroyPipeline(m_rendererContext.device, m_image_pipeline, nullptr);
    vkDestroyPipelineLayout(m_rendererContext.device, m_image_pipeline_layout, nullptr);
    vkDestroyRenderPass(m_rendererContext.device, m_main_render_pass, nullptr);
}

void CSJRenderSystem::createVertexBuffer() {
    std::vector<CSJVertex> current_vertices = obj_verteices;//vertices;
    VkDeviceSize bufferSize = sizeof(current_vertices[0]) * current_vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void *data;
    vkMapMemory(m_rendererContext.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, current_vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_rendererContext.device, stagingBufferMemory);

    createBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_vertex_buffer,
                 m_vertex_buffer_memory);
    
    copyBuffer(stagingBuffer, m_vertex_buffer, bufferSize);

    vkDestroyBuffer(m_rendererContext.device, stagingBuffer, nullptr);
    vkFreeMemory(m_rendererContext.device, stagingBufferMemory, nullptr);
}

void CSJRenderSystem::createIndexBuffer() {
    std::vector<uint32_t> currentIndices = obj_indices;

    VkDeviceSize bufferSize = sizeof(currentIndices[0]) * currentIndices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(m_rendererContext.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, currentIndices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_rendererContext.device, stagingBufferMemory);

    createBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_index_buffer, m_index_buffer_memory);

    copyBuffer(stagingBuffer, m_index_buffer, bufferSize);

    vkDestroyBuffer(m_rendererContext.device, stagingBuffer, nullptr);
    vkFreeMemory(m_rendererContext.device, stagingBufferMemory, nullptr);
}

void CSJRenderSystem::createTextureImage() {
    // resources/originImages/slamDumk_images/cross_street.jpeg
    int texWidth = 0, texHeight = 0, texChannels = 0;
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
    vkMapMemory(m_rendererContext.device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_rendererContext.device, stagingBufferMemory);
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

    vkDestroyBuffer(m_rendererContext.device, stagingBuffer, nullptr);
    vkFreeMemory(m_rendererContext.device, stagingBufferMemory, nullptr);

    generateMipmaps(m_image_texture_image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, m_mipLevels);
}

void CSJRenderSystem::loadModels() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<CSJVertex, uint32_t> uniqueVerteices{};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            CSJVertex vertex{};

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
            //obj_verteices.push_back(CSJVertex);
            //obj_indices.push_back(indices.size());
        }
    }

    // without index, the vertex total number is 11484!
    // std::cout << "*******************************************" << std::endl;
    // std::cout << "Model loading compeleted, total vertex number: " << obj_verteices.size() << std::endl;
    // std::cout << "*******************************************" << std::endl;
}

void CSJRenderSystem::createFramebuffers(std::vector<VkImageView>& swapFramebuffers) {
    int swapChainSize = swapFramebuffers.size();
    m_swapFramebuffers.resize(swapChainSize);

    for (size_t i = 0; i < swapChainSize; i++) {
        std::array<VkImageView, 2> attachments = {
            swapFramebuffers[i],
            m_depth_image_view
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_main_render_pass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_rendererContext.swapChainInfo.swapChainWidth;
        framebufferInfo.height = m_rendererContext.swapChainInfo.swapChainHeight;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_rendererContext.device, &framebufferInfo, nullptr, 
                                &m_swapFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create frame buffer!");
        }
    }
}

void CSJRenderSystem::releaseFramebuffers() {
    if (m_swapFramebuffers.size() == 0) {
        return ;
    }

    for (auto framebuffer : m_swapFramebuffers) {
       vkDestroyFramebuffer(m_rendererContext.device, framebuffer, nullptr);
    }
}

VkFormat CSJRenderSystem::findSupportedFormat(const std::vector<VkFormat>& candidates, 
                                             VkImageTiling tiling, 
                                             VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_rendererContext.physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat CSJRenderSystem::findDepthFormat() {
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT, 
        VK_FORMAT_D32_SFLOAT_S8_UINT, 
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    return findSupportedFormat(candidates, 
                               VK_IMAGE_TILING_OPTIMAL, 
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void CSJRenderSystem::createDepthResources(int width, int height) {
    VkFormat depthFormat = findDepthFormat();

    createImage(width, height, 1, depthFormat, 
                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_image, m_depth_image_memory);
    m_depth_image_view = createImageView(m_depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

void CSJRenderSystem::releaseDepthResources() {
    VkDevice device = m_rendererContext.device;
    vkDestroyImage(device, m_depth_image, nullptr);
    vkFreeMemory(device, m_depth_image_memory, nullptr);
    vkDestroyImageView(device, m_depth_image_view, nullptr);
}

void CSJRenderSystem::createMainRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format          = m_rendererContext.colorFormat;
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

    if (vkCreateRenderPass(m_rendererContext.device, &renderPassInfo, nullptr, &m_main_render_pass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void CSJRenderSystem::drawImage(VkCommandBuffer commandBuffer, double timeStamp, int index) {
    updateUniformBuffer(index);

    VkClearColorValue clearColor = {1.0f, 0.0f, 1.0f, 1.0f};
    VkClearDepthStencilValue clearDS = {1, 0};

    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo passBeginInfo{};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.renderPass = m_main_render_pass;
    passBeginInfo.framebuffer = m_swapFramebuffers[index];
    passBeginInfo.renderArea.extent.width = m_rendererContext.swapChainInfo.swapChainWidth;
    passBeginInfo.renderArea.extent.height = m_rendererContext.swapChainInfo.swapChainHeight;
    passBeginInfo.clearValueCount = 2;
    passBeginInfo.pClearValues = clearValues;
    vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_image_pipeline);

    VkViewport viewPort{};
    viewPort.x = viewPort.y = 0;
    viewPort.width = m_rendererContext.swapChainInfo.swapChainWidth;
    viewPort.height = m_rendererContext.swapChainInfo.swapChainHeight;
    viewPort.minDepth = 0;
    viewPort.maxDepth = 1;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewPort.width;
    scissor.extent.height = viewPort.height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {
        m_vertex_buffer
    };
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
                                          m_image_pipeline_layout, 0, 1, &m_image_descriptor_sets[index], 0, nullptr);
    
    //vkCmdDraw(commandBuffer, static_cast<uint32_t>(obj_verteices.size()), 1, 0, 0);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(obj_indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
}

void CSJRenderSystem::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    CSJUniformBufferObject ubo{};
    ubo.model    = glm::rotate(glm::mat4(1.0f), 0.0f/*time * glm::radians(90.0f)*/, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view     = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    float aspect = (float) m_rendererContext.swapChainInfo.swapChainWidth / (float) m_rendererContext.swapChainInfo.swapChainHeight;
    ubo.proj     = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_image_uniform_buffer_mappeds[currentImage], &ubo, sizeof(ubo));
}

void CSJRenderSystem::createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
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

    if (vkCreateImage(m_rendererContext.device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_rendererContext.device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_rendererContext.device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_rendererContext.device, image, imageMemory, 0);
}

VkImageView CSJRenderSystem::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
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
    if (vkCreateImageView(m_rendererContext.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    return imageView;
}

uint32_t CSJRenderSystem::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_rendererContext.physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void CSJRenderSystem::createBuffer(VkDeviceSize size, 
                                  VkBufferUsageFlags usage, 
                                  VkMemoryPropertyFlags properties, 
                                  VkBuffer &buffer, 
                                  VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_rendererContext.device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_rendererContext.device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_rendererContext.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_rendererContext.device, buffer, bufferMemory, 0);
}

void CSJRenderSystem::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_rendererContext.graphicsCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_rendererContext.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(m_rendererContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_rendererContext.graphicsQueue);

    vkFreeCommandBuffers(m_rendererContext.device, m_rendererContext.graphicsCommandPool, 1, &commandBuffer);
}

void CSJRenderSystem::copyBufferToImage(VkBuffer buffer, VkImage image,
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

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void CSJRenderSystem::transitionImageLayout(VkImage image, VkFormat format,
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

    vkCmdPipelineBarrier(commandBuffer,
                                       sourceStage, 
                                       destinationStage,
                                       0,0, nullptr,0,
                                       nullptr,1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void CSJRenderSystem::generateMipmaps(VkImage image, VkFormat format, 
                                     int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_rendererContext.physicalDevice, format, &formatProperties);
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

        vkCmdPipelineBarrier(commandBuffer, 
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

        vkCmdBlitImage(commandBuffer, 
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

    vkCmdPipelineBarrier(commandBuffer,
                                       VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
                                       0,
                                       0, nullptr,
                                       0, nullptr,
                                       1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer CSJRenderSystem::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_rendererContext.graphicsCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_rendererContext.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer; 
}

void CSJRenderSystem::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_rendererContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_rendererContext.graphicsQueue);

    vkFreeCommandBuffers(m_rendererContext.device, m_rendererContext.graphicsCommandPool, 1, &commandBuffer);
}

void CSJRenderSystem::updateRenderSystemContext(CSJRenderSystemContext &renderSystemContext) {
    m_rendererContext = renderSystemContext;
}

void CSJRenderSystem::updateSwapChainInfo(CSJSwapChainInfo& swapChainSize) {
    m_rendererContext.swapChainInfo = swapChainSize;
}

void CSJRenderSystem::loadDepthAndFrameBuffers(std::vector<VkImageView> &swapFramebuffers) {
    createDepthResources(m_rendererContext.swapChainInfo.swapChainWidth, 
                         m_rendererContext.swapChainInfo.swapChainHeight);

    createFramebuffers(swapFramebuffers);
}

void CSJRenderSystem::releaseDepthAndFrameBuffers() {
    releaseDepthResources();
    releaseFramebuffers();
}

}

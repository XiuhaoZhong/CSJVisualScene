#include "render_system.h"

#include <fstream>
#include <array>

//#include "framework/"
#include "Utils/CSJLogger.h"

// struct UniformBufferObject {
//     glm::mat4 model;
//     glm::mat4 view;
//     glm::mat4 proj;
// };

namespace CSJEngine {
CSJRenderSystem::~CSJRenderSystem() {
    m_pLogger = CSJLogger::getLoggerInst();
}

bool CSJRenderSystem::initialize(VkDevice device) {

    m_device = device;

    createDescriptorSetLayout();

    return true;
}

void CSJRenderSystem::render_tick(double timestamp) {

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
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }

    return shaderModule;
}

void CSJRenderSystem::createImageUniformBuffers() {
    // VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    // m_image_uniform_buffers.resize(m_max_frames_in_flight);
    // m_image_uniform_buffer_memories.resize(m_max_frames_in_flight);
    // m_image_uniform_buffer_mappeds.resize(m_max_frames_in_flight);

    // for (size_t i = 0 ; i < m_max_frames_in_flight; i++) {
    //     createBuffer(bufferSize,
    //                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    //                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    //                  m_image_uniform_buffers[i],
    //                  m_image_uniform_buffer_memories[i]);
        
    //     vkMapMemory(m_device, m_image_uniform_buffer_memories[i], 0, bufferSize, 0, &m_image_uniform_buffer_mappeds[i]);
    // }
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

    if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_image_descriptorset_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout");
    }
}

}
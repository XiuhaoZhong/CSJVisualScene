#ifndef __CSJRENDERSYSTEM_H__
#define __CSJRENDERSYSTEM_H__

#include <vector>
#include <string>

#include <vulkan/vulkan_core.h>

class CSJLogger;

namespace CSJEngine {

class CSJRenderSystem {
public:
    CSJRenderSystem() = default;
    ~CSJRenderSystem();

    bool initialize(VkDevice device);

    void render_tick(double timestamp); 

    void createDescriptorSetLayout();

protected:
    std::vector<char> readFile(const std::string &filename);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createImageUniformBuffers();
    // void createDescriptorPool();
    // void createDescriptorSetLayout();
    // void createDescriptorSets();
    // void createImageRenderPipeline();
    // void createImageTextureImageView();
    // void createImageTextureSampler();
    // void createImageResources();   
    // void releaseImageResources();
                    
    // void createVertexBuffer();
    // void createIndexBuffer();
    // void createTextureImage();

    // void loadModels();
    
    // void createFramebuffers();
    // void releaseFramebuffers();

    // VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, 
    //                              VkImageTiling tiling, 
    //                              VkFormatFeatureFlags features);
    // VkFormat findDepthFormat();
    // void createDepthResources();
    // void releaseDepthResources();

    // void createMainRenderPass();

    // void drawImage(VkCommandBuffer commandBuffer, int index);
    // void updateUniformBuffer(uint32_t currentImage);

    // VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    // void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
    //                  VkFormat format, VkImageTiling tiling, 
    //                  VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
    //                  VkImage &image, VkDeviceMemory &imageMemory);

    // uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    // void createBuffer(VkDeviceSize size, 
    //                   VkBufferUsageFlags usage, 
    //                   VkMemoryPropertyFlags properties, 
    //                   VkBuffer &buffer, 
    //                   VkDeviceMemory &bufferMemory);

    // void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    // void copyBufferToImage(VkBuffer buffer, VkImage image,
    //                        uint32_t width, uint32_t height);

    // void transitionImageLayout(VkImage image, VkFormat format,
    //                            VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    // void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    // VkCommandBuffer beginSingleTimeCommands();
    // void endSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
    CSJLogger              *m_pLogger;

    std::vector<VkBuffer>        m_image_uniform_buffers;
    std::vector<VkDeviceMemory>  m_image_uniform_buffer_memories;
    std::vector<void *>          m_image_uniform_buffer_mappeds;
    uint32_t                     m_mipLevels;
    VkDescriptorSetLayout   m_image_descriptorset_layout;
    VkDevice                m_device;
};

}
#endif // __CSJRENDERSYSTEM_H__
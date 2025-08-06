#ifndef __CSJRENDERSYSTEM_H__
#define __CSJRENDERSYSTEM_H__

#include <vector>
#include <string>
#include <array>

#include <vulkan/vulkan_core.h>

class CSJLogger;

namespace CSJEngine {

struct CSJSwapChainInfo {
    int max_frames_in_flight;
    int swapChainWidth;
    int swapChainHeight;

    CSJSwapChainInfo& operator=(CSJSwapChainInfo& swapChain) {
        this->max_frames_in_flight = swapChain.max_frames_in_flight;
        this->swapChainWidth       = swapChain.swapChainWidth;
        this->swapChainHeight      = swapChain.swapChainHeight;

        return *this;
    }

};// CSJSwapChainInfo;

/* Get these context data from QVulkanWindow. */
struct CSJRenderSystemContext {
    CSJSwapChainInfo    swapChainInfo;
    VkQueue             graphicsQueue;
    VkCommandPool       graphicsCommandPool;
    VkCommandBuffer     commandBuffer;
    VkFormat            colorFormat;
    VkDevice            device;
    VkPhysicalDevice    physicalDevice;

    CSJRenderSystemContext& operator=(CSJRenderSystemContext& renderSystemContext) {
        this->swapChainInfo        = renderSystemContext.swapChainInfo;
        this->graphicsQueue        = renderSystemContext.graphicsQueue;
        this->graphicsCommandPool  = renderSystemContext.graphicsCommandPool;
        this->commandBuffer        = renderSystemContext.commandBuffer;
        this->colorFormat          = renderSystemContext.colorFormat;
        this->device               = renderSystemContext.device;
        this->physicalDevice       = renderSystemContext.physicalDevice;

        return *this;
    }
    
};// CSJRenderSystemContext;

class CSJRenderSystem {
public:
    CSJRenderSystem();
    ~CSJRenderSystem();

    bool isInit();
    bool initialize();
    void updateRenderSystemContext(CSJRenderSystemContext& renderSystemContext);
    void updateSwapChainInfo(CSJSwapChainInfo& swapChainInfo);

    void loadDepthAndFrameBuffers(std::vector<VkImageView>& swapFramebuffers);
    void releaseDepthAndFrameBuffers();

    void render_tick(VkCommandBuffer commandBuffer, double timestamp, int swapFrameIndex); 

protected:
    std::vector<char> readFile(const std::string &filename);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createImageUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createDescriptorSets();
    void createImageRenderPipeline();
    void createImageTextureImageView();
    void createImageTextureSampler();
    void createImageResources();   
    void releaseImageResources();
    void createMainRenderPass();
                    
    void createVertexBuffer();
    void createIndexBuffer();
    void createTextureImage();

    void loadModels();
    
    void createDepthResources(int width, int height);
    void releaseDepthResources();
    void createFramebuffers(std::vector<VkImageView>& swapFramebuffers);
    void releaseFramebuffers();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, 
                                 VkImageTiling tiling, 
                                 VkFormatFeatureFlags features);
    VkFormat findDepthFormat();

    void drawImage(VkCommandBuffer commandBuffer, double timestamp, int index);
    void updateUniformBuffer(uint32_t currentImage);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                     VkFormat format, VkImageTiling tiling, 
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkImage &image, VkDeviceMemory &imageMemory);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, 
                      VkBufferUsageFlags usage, 
                      VkMemoryPropertyFlags properties, 
                      VkBuffer &buffer, 
                      VkDeviceMemory &bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void copyBufferToImage(VkBuffer buffer, VkImage image,
                           uint32_t width, uint32_t height);

    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
    CSJLogger                   *m_pLogger;

    VkImage                      m_depth_image;
    VkImageView                  m_depth_image_view;
    VkDeviceMemory               m_depth_image_memory;
    std::vector<VkFramebuffer>   m_swapFramebuffers;

    VkBuffer                     m_vertex_buffer;
    VkDeviceMemory               m_vertex_buffer_memory;
    VkBuffer                     m_index_buffer;
    VkDeviceMemory               m_index_buffer_memory;

    std::vector<VkBuffer>        m_image_uniform_buffers;
    std::vector<VkDeviceMemory>  m_image_uniform_buffer_memories;
    std::vector<void *>          m_image_uniform_buffer_mappeds;
    uint32_t                     m_mipLevels;
    std::vector<VkDescriptorSet> m_image_descriptor_sets; 
    VkDescriptorSetLayout        m_image_descriptorset_layout;
    VkDescriptorPool             m_image_descriptor_pool;
    VkSampler                    m_image_texture_sampler;
    VkImage                      m_image_texture_image;
    VkDeviceMemory               m_image_texture_image_memory;
    VkImageView                  m_image_texture_imageview;
    VkPipelineLayout             m_image_pipeline_layout;
    VkPipeline                   m_image_pipeline;
    VkRenderPass                 m_main_render_pass;

    CSJRenderSystemContext       m_rendererContext;
    bool                         m_init;
};

}
#endif // __CSJRENDERSYSTEM_H__
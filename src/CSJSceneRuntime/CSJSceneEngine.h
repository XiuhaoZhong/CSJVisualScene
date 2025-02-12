#ifndef __CSJSCENEENGINE_H__
#define __CSJSCENEENGINE_H__

#include <memory>

#include <QVulkanWindow>
#include <QVulkanDeviceFunctions>
#include <vulkan/vulkan_core.h>

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
                    
    void createVertexBuffer();
    void createIndexBuffer();
    void createTextureImage();
    
    
    void createFramebuffers();
    void releaseFramebuffers();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, 
                                 VkImageTiling tiling, 
                                 VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    void createDepthResources();
    void releaseDepthResources();


    void createMainRenderPass();

    void drawImage(VkCommandBuffer commandBuffer, int index);
    void updateUniformBuffer(uint32_t currentImage);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createImage(uint32_t width, uint32_t height, 
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
                               VkImageLayout oldLayout, VkImageLayout newLayout);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
private:
    QVulkanWindow          *m_pWindow;
    QVulkanFunctions       *m_pVulkanFunctions;
    QVulkanDeviceFunctions *m_pFunctions;
    uint32_t                m_max_frames_in_flight;

    std::vector<VkBuffer>        m_image_uniform_buffers;
    std::vector<VkDeviceMemory>  m_image_uniform_buffer_memories;
    std::vector<void *>          m_image_uniform_buffer_mappeds;

    VkBuffer                     m_vertex_buffer;
    VkDeviceMemory               m_vertex_buffer_memory;
    VkBuffer                     m_index_buffer;
    VkDeviceMemory               m_index_buffer_memory;
    VkImage                      m_image_texture_image;
    VkDeviceMemory               m_image_texture_image_memory;

    VkSampler                    m_image_texture_sampler;
    VkImageView                  m_image_texture_imageview;

    std::vector<VkDescriptorSet> m_image_descriptor_sets; 
    VkDescriptorSetLayout        m_image_descriptorset_layout;
    VkDescriptorPool             m_image_descriptor_pool;
    VkPipelineLayout             m_image_pipeline_layout;
    VkPipeline                   m_image_pipeline;
    
    std::vector<VkFramebuffer>   m_framebuffers;
    VkImage                      m_depth_image;
    VkDeviceMemory               m_depth_image_memory;
    VkImageView                  m_depth_image_view;    

    VkRenderPass                 m_main_render_pass;

    bool m_isInit;
};

using CSJSpGLRenderWidget = std::shared_ptr<CSJSceneEngine>;

#endif // __CSJSCENEENGINE_H__
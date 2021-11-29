#pragma once 

#ifndef VULKAN_HELPERS
#define VULKAN_HELPERS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"
#include <vector>

struct GraphicsPipeline {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct AllocatedImage {
    VkImage image;
    VmaAllocation allocation;
};

namespace vkInit {

    VkBufferCreateInfo getBufferCreateinfo(VkDeviceSize bufferSize,
        VkBufferUsageFlags usageFlags);

    VkImageCreateInfo getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
        uint32_t mipLevels = 1, uint32_t arrayLayers = 1);

    VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image,
        VkImageAspectFlags aspectFlags);

    VkFramebufferCreateInfo getFrameBufferCreateInfo(VkRenderPass renderPass, VkExtent2D extent, 
        uint32_t attachmentCount, VkImageView* attachments);

    VkDescriptorSetLayoutBinding getDescripterLayoutBindingInfo(VkDescriptorType type,
        VkShaderStageFlags stageFlags, uint32_t binding);

    VkWriteDescriptorSet writeDescriptorSet(VkDescriptorType type, VkDescriptorSet dstSet,
        VkDescriptorBufferInfo* bufferInfo, uint32_t binding);

    VkDescriptorSetAllocateInfo getDescriptorAllocInfo(VkDescriptorPool pool, VkDescriptorSetLayout* setLayouts,
        uint32_t setCount);

    VkSubmitInfo getSubmitInfo(::std::vector<VkSemaphore>& waitSemaphores, ::std::vector<VkSemaphore>& signalSemaphores,
        VkPipelineStageFlags* waitStages, ::std::vector<VkCommandBuffer>& commandBuffers);

    VkPresentInfoKHR getPresentInfo(::std::vector<VkSemaphore>& waitSemaphores, ::std::vector<VkSwapchainKHR>& swapChains,
        uint32_t* nextImageIndices);
}



#endif // !VULKAN_HELPERS

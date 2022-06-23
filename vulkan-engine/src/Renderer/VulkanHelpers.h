#pragma once 

#ifndef VULKAN_HELPERS
#define VULKAN_HELPERS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"
#include <vector>

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)                 

class VulkanDevice;

namespace vk {

    enum class ResourceState {
        UNINITIALIZED = 0,
        READY,
        DESTROYED
    };

    struct GraphicsPipeline {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        VkDescriptorSetLayout GlobalUniformLayout;
        VkDescriptorSetLayout PerObjectLayout;
    };

    struct Buffer {
        VkBuffer handle;
        VmaAllocation allocation;
        VmaAllocator* allocatorRef;
        VmaMemoryUsage memoryUsage;
        VkBufferUsageFlags usageFlags;
        ResourceState state;

        void create(uint64_t bufferSize,
            VkBufferUsageFlags usageFlags,
            VmaMemoryUsage usage);
        void destroy();
        void copyData(void* data, size_t size);
        void copyToBuffer(Buffer& dest, 
            uint64_t srcOffset,
            uint64_t destOffset,
            uint64_t size);
    };

    struct Image {
        VkImage handle;
        VmaAllocation allocation;
        VmaAllocator* allocatorRef;
        //ResourceState state;
        VkExtent3D extent;
        VkFormat format;
        uint32_t mipLevels;
        uint32_t layers;
    };

    struct Texture {
        Image image;
        VkSampler sampler;
        VkImageView view;
        VkImageViewType viewType;
        VulkanDevice* deviceRef;

        bool hasSampler;

        void create(VkFormat format,
            VkImageUsageFlags usageFlags,
            VkExtent3D extent,
            uint32_t mipLevels,
            uint32_t arrayLayers);
        void createView(VkImageAspectFlags aspectFlags, 
            VkImageViewType viewType);
        void createSampler(VkFilter minFilter,
            VkFilter magFilter,
            VkSamplerAddressMode samplerAddressMode);
        void destroy();
        //void transitionLayout(VkCommandBuffer cmd,
        //    VkImageSubresourceRange range,
        //    VkImageLayout oldLayout,
        //    VkImageLayout newLayout,
        //    )
        void copyFromBuffer(Buffer& srcbuffer);
    };

    //VkBufferCreateInfo getBufferCreateinfo(VkDeviceSize bufferSize,
    //    VkBufferUsageFlags usageFlags);

    //VkImageCreateInfo getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
    //    uint32_t mipLevels = 1, uint32_t arrayLayers = 1);

    //VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image,
    //    VkImageAspectFlags aspectFlags);

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

    //VkImageMemoryBarrier getIMageMemoryBarrierInfo(VkImageLayout newLayout, VkImage dstImage, VkImageSubresourceRange range,
    //    VkAccessFlags dstAccessMask, VkAccessFlags srcAccessMask = 0);

    //VkImageMemoryBarrier getImageTransitionInfo(VkImage dstImage, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange range, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

    //VkSamplerCreateInfo getSamplerCreateInfo(VkFilter filters, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

    //VkBufferImageCopy getBufferImageCopyInfo(VkImageAspectFlags aspectmask, VkExtent3D extents);
}



#endif // !VULKAN_HELPERS

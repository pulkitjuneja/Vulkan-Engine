#pragma once 

#ifndef VULKAN_HELPERS
#define VULKAN_HELPERS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"
#include <vector>
#include <functional>
#include <map>

const int MAX_FRAMES_IN_FLIGHT = 2;

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

namespace utils {
    template <class T>
    constexpr void hashCombine(int& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}

namespace vk {

    enum class ResourceState {
        UNINITIALIZED = 0,
        READY,
        DESTROYED
    };


    struct Buffer {
        VkBuffer handle;
        VmaAllocation allocation;
        VmaAllocator* allocatorRef;
        VmaMemoryUsage memoryUsage;
        VkBufferUsageFlags usageFlags;
        uint64_t bufferSize;
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
        VkImageLayout layout;
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

    struct Descriptor {
        uint32_t binding;
        VkDescriptorType type;
        uint32_t stageFlags;

        Buffer* buffer;
        Texture* texture;
        uint64_t offset;
        uint64_t range;

        int computeHash();
    };

    struct DescriptorSetLayout;

    struct DescriptorSet {
        VkDescriptorSet handle;
        VulkanDevice* deviceRef;

        void create(DescriptorSetLayout &layout);
        void destroy();
        void update(std::vector<Descriptor>& descriptors);
    };

    struct DescriptorSetLayout {
        VkDescriptorSetLayout handle;
        VulkanDevice* deviceRef;
        //store descriptors with their slot for quick access
        std::map<uint32_t, Descriptor> descriptors;

        void create(std::vector<Descriptor> descriptors);
        void destroy();
        int computeHash();
        
        //use the descriptor information to create a new descriptor set
        // could move this to the descriptorset struct itself
        void setBuffer(uint32_t binding, Buffer* buffer, uint64_t offset, uint64_t range);
        void setTexture(uint32_t binding, Texture* texture);
        DescriptorSet* getDescriptorSet(DescriptorSet& outSet);
    };

    struct CommandBuffer {
        VkCommandBuffer handle;
        VulkanDevice* deviceRef;

        void create(VkCommandPool& commandPool);
        void beginRecording(VkCommandBufferUsageFlags flags);
        void endRecording();
        void Draw(int vertexCount);

        // TODO : move these to the renderpass class later on
        void beginRenderPass(VkRenderPass& renderPass, VkFramebuffer& frameBuffer, VkExtent2D& extents);
        void endRenderPass();

        static void submit(std::vector<VkSemaphore>& waitSemaphores,
            std::vector<VkSemaphore>& signalSemaphores,
            VkPipelineStageFlags* waitStages,
            std::vector<VkCommandBuffer>& commandBuffers,
            VkFence frameFence);
    };

    struct GraphicsPipeline {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        DescriptorSetLayout GlobalUniformLayout;
        DescriptorSetLayout PerObjectLayout;

        void bind(CommandBuffer cmd);
    };


    //VkBufferCreateInfo getBufferCreateinfo(VkDeviceSize bufferSize,
    //    VkBufferUsageFlags usageFlags);

    //VkImageCreateInfo getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
    //    uint32_t mipLevels = 1, uint32_t arrayLayers = 1);

    //VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image,
    //    VkImageAspectFlags aspectFlags);

   /* VkDescriptorSetLayoutBinding getDescripterLayoutBindingInfo(VkDescriptorType type,
        VkShaderStageFlags stageFlags, uint32_t binding);

    VkWriteDescriptorSet writeDescriptorSet(VkDescriptorType type, VkDescriptorSet dstSet,
        VkDescriptorBufferInfo* bufferInfo, uint32_t binding);

    VkDescriptorSetAllocateInfo getDescriptorAllocInfo(VkDescriptorPool pool, VkDescriptorSetLayout* setLayouts,
        uint32_t setCount);*/

    VkFramebufferCreateInfo getFrameBufferCreateInfo(VkRenderPass renderPass, VkExtent2D extent,
        uint32_t attachmentCount, VkImageView* attachments);

    //VkSubmitInfo getSubmitInfo(::std::vector<VkSemaphore>& waitSemaphores, ::std::vector<VkSemaphore>& signalSemaphores,
    //    VkPipelineStageFlags* waitStages, ::std::vector<VkCommandBuffer>& commandBuffers);

    //VkPresentInfoKHR getPresentInfo(::std::vector<VkSemaphore>& waitSemaphores, ::std::vector<VkSwapchainKHR>& swapChains,
    //    uint32_t* nextImageIndices);

    //VkImageMemoryBarrier getIMageMemoryBarrierInfo(VkImageLayout newLayout, VkImage dstImage, VkImageSubresourceRange range,
    //    VkAccessFlags dstAccessMask, VkAccessFlags srcAccessMask = 0);

    //VkImageMemoryBarrier getImageTransitionInfo(VkImage dstImage, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange range, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

    //VkSamplerCreateInfo getSamplerCreateInfo(VkFilter filters, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

    //VkBufferImageCopy getBufferImageCopyInfo(VkImageAspectFlags aspectmask, VkExtent3D extents);
}

template<class T>
struct PerFrameData {
    T objects[MAX_FRAMES_IN_FLIGHT];
    T& operator[] (int& index) { return objects[index]; }
    T& operator[] (size_t& index) { return objects[index]; }

    void forEach(std::function<void(T& object)>&& func) {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            func(objects[i]);
        }
    }

    void forEach(std::function<void(T& object, int index)>&& func) {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            func(objects[i], i);
        }
    }
};




#endif // !VULKAN_HELPERS

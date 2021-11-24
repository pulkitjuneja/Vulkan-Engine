#pragma once 

#ifndef VULKAN_HELPERS
#define VULKAN_HELPERS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"


struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;

    void createBuffer(VkDeviceSize bufferSize,
        VkBufferUsageFlags usageFlags, VmaMemoryUsage memUsageFlags);
};

struct AllocatedImage {
    VkImage image;
    VmaAllocation allocation;

    void createImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
        VmaMemoryUsage memUsageFlags, uint32_t mipLevels = 1, uint32_t arrayLayers = 1);
};

VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image,
    VkImageAspectFlags aspectFlags);



#endif // !VULKAN_HELPERS

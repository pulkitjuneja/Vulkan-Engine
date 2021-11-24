#include "VulkanHelpers.h"
#include "EngineContext.h"
#include "VulkanContext.h" 


void AllocatedBuffer::createBuffer(VkDeviceSize bufferSize,
    VkBufferUsageFlags usageFlags, VmaMemoryUsage memUsageFlags)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;

    VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = memUsageFlags;

    if (vmaCreateBuffer(alloc, &bufferInfo, &vmaallocInfo, &buffer,
        &allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer for mesh");
    }
}

void AllocatedImage::createImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
    VmaMemoryUsage memUsageFlags, uint32_t mipLevels, uint32_t arrayLayers)
{
    VkImageCreateInfo info = { };
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = mipLevels;
    info.arrayLayers = arrayLayers;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    VmaAllocationCreateInfo allocinfo = {};
    allocinfo.usage = memUsageFlags;
    allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;

    if (vmaCreateImage(alloc, &info, &allocinfo, &image, &allocation, nullptr)) {
        Logger::logError("Error creating image");
    }

}
VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;

    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspectFlags;

    return info;
}

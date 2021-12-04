#include "VulkanHelpers.h"
#include "EngineContext.h"
#include "VulkanContext.h" 

namespace vkInit {

    VkBufferCreateInfo getBufferCreateinfo(VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usageFlags;

        return bufferInfo;
    }

    VkImageCreateInfo getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, 
        uint32_t mipLevels, uint32_t arrayLayers)
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

        return info;
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
    VkFramebufferCreateInfo getFrameBufferCreateInfo(VkRenderPass renderPass, VkExtent2D extent, uint32_t attachmentCount,
        VkImageView* attachments)
    {
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = renderPass;
        info.attachmentCount = attachmentCount;
        info.pAttachments = attachments;
        info.width = extent.width;
        info.height = extent.height;
        info.layers = 1;

        return info;
    }
    VkDescriptorSetLayoutBinding getDescripterLayoutBindingInfo(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
    {
        VkDescriptorSetLayoutBinding setbind = {};
        setbind.binding = binding;
        setbind.descriptorCount = 1;
        setbind.descriptorType = type;
        setbind.pImmutableSamplers = nullptr;
        setbind.stageFlags = stageFlags;

        return setbind;
    }
    VkWriteDescriptorSet writeDescriptorSet(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding)
    {
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;
        write.dstBinding = binding;
        write.dstSet = dstSet;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = bufferInfo;

        return write;
    }
    VkDescriptorSetAllocateInfo getDescriptorAllocInfo(VkDescriptorPool pool, VkDescriptorSetLayout* setLayouts,
        uint32_t setCount)
    {
        VkDescriptorSetAllocateInfo info = {};
        info.pNext = nullptr;
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = pool;
        info.descriptorSetCount = setCount;
        info.pSetLayouts = setLayouts;

        return info;
    }
    VkSubmitInfo getSubmitInfo(::std::vector<VkSemaphore>& waitSemaphores, ::std::vector<VkSemaphore>& signalSemaphores, 
        VkPipelineStageFlags* waitStages, ::std::vector<VkCommandBuffer>& commandBuffers)
    {
        VkSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.waitSemaphoreCount = waitSemaphores.size();
        info.pWaitSemaphores = waitSemaphores.size() > 0 ? waitSemaphores.data() : nullptr;
        info.pWaitDstStageMask = waitStages;
        info.commandBufferCount = commandBuffers.size();
        info.pCommandBuffers = commandBuffers.data();
        info.signalSemaphoreCount = signalSemaphores.size();
        info.pSignalSemaphores = signalSemaphores.size() > 0 ? signalSemaphores.data() : nullptr;

        return info;
    }
    VkPresentInfoKHR getPresentInfo(::std::vector<VkSemaphore>& waitSemaphores, ::std::vector<VkSwapchainKHR>& swapChains,
        uint32_t* nextImageIndices)
    {
        VkPresentInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = waitSemaphores.size();
        info.pWaitSemaphores = waitSemaphores.data();
        info.swapchainCount = swapChains.size();
        info.pSwapchains = swapChains.data();
        info.pImageIndices = nextImageIndices;

        return info;
    }
    VkImageMemoryBarrier getIMageMemoryBarrierInfo(VkImageLayout newLayout, VkImage dstImage, VkImageSubresourceRange range, 
        VkAccessFlags dstAccessMask, VkAccessFlags srcAccessMask)
    {
        VkImageMemoryBarrier info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        info.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.newLayout = newLayout;
        info.image = dstImage;
        info.subresourceRange = range;
        info.srcAccessMask = srcAccessMask;
        info.dstAccessMask = dstAccessMask;

        return info;
    }
    VkBufferImageCopy getBufferImageCopyInfo(VkImageAspectFlags aspectmask, VkExtent3D extents)
    {
        VkBufferImageCopy info = {};
        info.bufferOffset = 0;
        info.bufferRowLength = 0;
        info.bufferImageHeight = 0;
        info.imageSubresource.aspectMask = aspectmask;
        info.imageSubresource.mipLevel = 0;
        info.imageSubresource.baseArrayLayer = 0;
        info.imageSubresource.layerCount = 1;
        info.imageExtent = extents;

        return info;
    }
}

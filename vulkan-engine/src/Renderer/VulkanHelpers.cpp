#include "VulkanHelpers.h"
#include "EngineContext.h"
#include "VulkanContext.h" 

namespace vk {

    //VkBufferCreateInfo getBufferCreateinfo(VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags)
    //{
    //    VkBufferCreateInfo bufferInfo = {};
    //    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //    bufferInfo.size = bufferSize;
    //    bufferInfo.usage = usageFlags;

    //    return bufferInfo;
    //}

    //VkImageCreateInfo getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, 
    //    uint32_t mipLevels, uint32_t arrayLayers)
    //{
    //    VkImageCreateInfo info = { };
    //    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    //    info.pNext = nullptr;
    //    info.imageType = VK_IMAGE_TYPE_2D;
    //    info.format = format;
    //    info.extent = extent;
    //    info.mipLevels = mipLevels;
    //    info.arrayLayers = arrayLayers;
    //    info.samples = VK_SAMPLE_COUNT_1_BIT;
    //    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    //    info.usage = usageFlags;

    //    return info;
    //}

    //VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
    //{
    //    VkImageViewCreateInfo info = {};
    //    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //    info.pNext = nullptr;

    //    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    //    info.image = image;
    //    info.format = format;
    //    info.subresourceRange.baseMipLevel = 0;
    //    info.subresourceRange.levelCount = 1;
    //    info.subresourceRange.baseArrayLayer = 0;
    //    info.subresourceRange.layerCount = 1;
    //    info.subresourceRange.aspectMask = aspectFlags;

    //    return info;
    //}
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
    //VkImageMemoryBarrier getIMageMemoryBarrierInfo(VkImageLayout newLayout, VkImage dstImage, VkImageSubresourceRange range, 
    //    VkAccessFlags dstAccessMask, VkAccessFlags srcAccessMask)
    //{
    //    VkImageMemoryBarrier info = {};
    //    info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    //    info.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    info.newLayout = newLayout;
    //    info.image = dstImage;  
    //    info.subresourceRange = range;
    //    info.srcAccessMask = srcAccessMask;
    //    info.dstAccessMask = dstAccessMask;

    //    return info;
    //}

    //VkImageMemoryBarrier getImageTransitionInfo(VkImage dstImage, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange range, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
    //{
    //    VkImageMemoryBarrier info = {};
    //    info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    //    info.oldLayout = oldLayout;
    //    info.newLayout = newLayout;
    //    info.image = dstImage;
    //    info.subresourceRange = range;
    //    info.srcAccessMask = srcAccessMask;
    //    info.dstAccessMask = dstAccessMask;

    //    return info;
    //}
    //VkSamplerCreateInfo getSamplerCreateInfo(VkFilter filters, VkSamplerAddressMode samplerAddressMode)
    //{
    //    VkSamplerCreateInfo info = {};
    //    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //    info.pNext = nullptr;

    //    info.magFilter = filters;
    //    info.minFilter = filters;
    //    info.addressModeU = samplerAddressMode;
    //    info.addressModeV = samplerAddressMode;
    //    info.addressModeW = samplerAddressMode;

    //    return info;
    //}
    //VkBufferImageCopy getBufferImageCopyInfo(VkImageAspectFlags aspectmask, VkExtent3D extents)
    //{
    //    VkBufferImageCopy info = {};
    //    info.bufferOffset = 0;
    //    info.bufferRowLength = 0;
    //    info.bufferImageHeight = 0;
    //    info.imageSubresource.aspectMask = aspectmask;
    //    info.imageSubresource.mipLevel = 0;
    //    info.imageSubresource.baseArrayLayer = 0;
    //    info.imageSubresource.layerCount = 1;
    //    info.imageExtent = extents;

    //    return info;
    //}



    // ---------------------------------- Buffer ------------------------------------
    void Buffer::create(uint64_t bufferSize, VkBufferUsageFlags usageFlags, VmaMemoryUsage memUsage)
    {
        allocatorRef = &EngineContext::get()->vulkanContext->allocator;
        memoryUsage = memUsage;
        this->usageFlags = usageFlags;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usageFlags;

        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = memoryUsage;
        if (vmaCreateBuffer(*allocatorRef, &bufferInfo, &vmaallocInfo, &handle, &allocation, nullptr) != VK_SUCCESS) {
            Logger::logError("failed to create  buffer");
        }
        state = ResourceState::READY;
    }

    void Buffer::destroy()
    {
        if (state == ResourceState::READY) {
            vmaDestroyBuffer(*allocatorRef, handle, allocation);
            state = ResourceState::DESTROYED;
        }
    }

    void Buffer::copyData(void* data, size_t size)
    {
        if (state != ResourceState::READY) {
            Logger::logError("Buffer not initialized");
            return;
        }
        if (memoryUsage == VMA_MEMORY_USAGE_GPU_ONLY) {
            Logger::logError("Cannot map Gpu only buffer");
            return;
        }
        void* mappedMemory;
        vmaMapMemory(*allocatorRef, allocation, &mappedMemory);
        memcpy(mappedMemory, data, size);
        vmaUnmapMemory(*allocatorRef, allocation);
    }

    void Buffer::copyToBuffer(Buffer& dest, uint64_t srcOffset, uint64_t destOffset, uint64_t size)
    {
        if (state != ResourceState::READY) {
            Logger::logError("Buffer not initialized");
            return;
        }
        if (!(usageFlags & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
            Logger::logError("Current Buffer dosent support transfer SRC operation");
            return;
        }
        if (!(dest.usageFlags & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
            Logger::logError("Current Buffer dosent support transfer DST operation");
            return;
        }

        auto& context = EC::get()->vulkanContext;
        context->immediateSubmit([=](VkCommandBuffer cmd) {
            VkBufferCopy copy;
            copy.dstOffset = srcOffset;
            copy.srcOffset = destOffset;
            copy.size = size;
            vkCmdCopyBuffer(cmd, handle, dest.handle, 1, &copy);
            });
    }


    // ---------------------------------- Image ------------------------------------
    void Texture::create(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, uint32_t mipLevels, uint32_t arrayLayers) {
        this->image.allocatorRef = &EngineContext::get()->vulkanContext->allocator;
        this->image.format = format;
        this->deviceRef = &EC::get()->vulkanContext->getDevice();
        this->image.extent = extent;
        this->image.mipLevels = mipLevels;
        this->image.layers = arrayLayers;

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

        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (vmaCreateImage(*image.allocatorRef, &info, &vmaallocInfo, &image.handle, &image.allocation, nullptr) != VK_SUCCESS) {
            Logger::logError("Error creating vulkan image");
            return;
        }
    }

    void Texture::destroy() {
        if (hasSampler) {
            vkDestroySampler(deviceRef->logicalDevice, sampler, nullptr);
            hasSampler = false;
        }
       vkDestroyImageView(deviceRef->logicalDevice, view, nullptr);
       vmaDestroyImage(*image.allocatorRef, image.handle, image.allocation);
    }

    void Texture::copyFromBuffer(Buffer& srcbuffer)
    {
        auto& context = EC::get()->vulkanContext;
        deviceRef = &EC::get()->vulkanContext->getDevice();
        // TODO : This will need to be configurable for more complex image types
        VkImageSubresourceRange range;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        VkImageMemoryBarrier imageBarrier_toTransfer = {};
        imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier_toTransfer.image = image.handle;
        imageBarrier_toTransfer.subresourceRange = range;
        imageBarrier_toTransfer.srcAccessMask = 0;
        imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        VkImageMemoryBarrier imageBarrier_toReadable = {};
        imageBarrier_toReadable.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageBarrier_toReadable.image = image.handle;
        imageBarrier_toReadable.subresourceRange = range;
        imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        context->immediateSubmit([&](VkCommandBuffer cmd) {
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

            VkBufferImageCopy copyRegion = {};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = image.extent;
            vkCmdCopyBufferToImage(cmd, srcbuffer.handle, image.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                nullptr, 1, &imageBarrier_toReadable);
        });
    }

    void Texture::createSampler(VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode samplerAddressMode)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext = nullptr;
        samplerInfo.magFilter = magFilter;
        samplerInfo.minFilter = minFilter;
        samplerInfo.addressModeU = samplerAddressMode;
        samplerInfo.addressModeV = samplerAddressMode;
        samplerInfo.addressModeW = samplerAddressMode;
        vkCreateSampler(deviceRef->logicalDevice, &samplerInfo, nullptr, &sampler);
        hasSampler = true;
    }

    void Texture::createView(VkImageAspectFlags aspectFlags, VkImageViewType viewType) {
        this->viewType = viewType;

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext = nullptr;
        viewInfo.viewType = viewType;
        viewInfo.image = image.handle;
        viewInfo.format = image.format;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.aspectMask = aspectFlags;

        if (vkCreateImageView(deviceRef->logicalDevice, &viewInfo, nullptr, &view) != VK_SUCCESS) {
            Logger::logError("Error creating image view");
        }
    }
}


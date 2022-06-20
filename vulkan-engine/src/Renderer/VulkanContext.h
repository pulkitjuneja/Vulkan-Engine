#pragma once

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "vk_mem_alloc.h"

#include "Window.h"
#include <vector>
#include <functional>
#include "Logger.h"
#include "VulkanDevice.h"
#include "EngineContext.h"
#include "VulkanSwapChain.h"
#include "VulkanInstance.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct FrameData {
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VulkanCommandBuffer FrameCommandBuffer;
	VkFence inFlightFence;

	AllocatedBuffer frameUniforms;
	VkDescriptorSet frameDescriptor;

	AllocatedBuffer objectBuffer;
	VkDescriptorSet objectDescriptor;
};

struct immediateSubmitContext {
	VkFence fence;
	VkCommandPool commandPool;
};

class VulkanContext {
protected:
	VulkanInstance instance;
	VulkanDevice device;
	VkSurfaceKHR surface;
	VulkanSwapChain swapChain;
	VkCommandPool graphicsCommandPool;


public:
	VmaAllocator allocator;
	std::vector<FrameData> frames;
	immediateSubmitContext context;

	VulkanContext() = default;
	void initialize();
	void release();

	VulkanInstance& getInstance() { return instance; }
	VulkanDevice& getDevice() { return device; }
	VkSurfaceKHR& getSurface() { return surface; }
	VulkanSwapChain& getSwapChain() { return swapChain; }
	VkCommandPool& getGraphicsCommandPool() { return graphicsCommandPool; }

	// main descriptor pool
	VkDescriptorPool descriptorPool;

	void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

	void initializeFrameData();
	void releaseFrameData();
	void createSurface();
	void createVMAllocator();
	void creategraphicsPool();
};

#endif // !VULKAN_CONTEXT_H

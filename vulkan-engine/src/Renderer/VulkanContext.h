#pragma once

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "vk_mem_alloc.h"

#include "Window.h"
#include <vector>
#include "Logger.h"
#include "VulkanDevice.h"
#include "EngineContext.h"
#include "VulkanSwapChain.h"
#include "VulkanInstance.h"

class VulkanContext {
protected:
	VulkanInstance instance;
	VulkanDevice device;
	VkSurfaceKHR surface;
	VulkanSwapChain swapChain;
	VkCommandPool graphicsCommandPool;

public:
	VmaAllocator allocator;
	VulkanContext() = default;
	void initialize();
	void release();

	VulkanInstance& getInstance() { return instance; }
	VulkanDevice& getDevice() { return device; }
	VkSurfaceKHR& getSurface() { return surface; }
	VulkanSwapChain& getSwapChain() { return swapChain; }
	VkCommandPool& getGraphicsCommandPool() { return graphicsCommandPool; }

	void createSurface();
	void createVMAllocator();
	void creategraphicsPool();
};

#endif // !VULKAN_CONTEXT_H

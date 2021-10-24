#pragma once

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

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
	VulkanContext() = default;
	void initialize();
	void release();

	VulkanInstance& getInstance() { return instance; }
	VulkanDevice& getDevice() { return device; }
	VkSurfaceKHR& getSurface() { return surface; }
	VulkanSwapChain& getSwapChain() { return swapChain; }
	VkCommandPool& getGraphicsCommandPool() { return graphicsCommandPool; }

	void createSurface();
	void creategraphicsPool();
};

#endif // !VULKAN_CONTEXT_H

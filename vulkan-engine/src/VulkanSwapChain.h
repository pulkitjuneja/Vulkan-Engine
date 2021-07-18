#pragma once

#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include "Window.h"
#include <vector>
#include "VulkanDevice.h"
#include "EngineContext.h"

class VulkanSwapChain {
protected:
	VkSurfaceCapabilitiesKHR capabilities;
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
public:
	void initialize(const VulkanDevice& device, const VkSurfaceKHR surface);
	void release(const VulkanDevice& device);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VulkanDevice& device, const VkSurfaceKHR surface);
	VkPresentModeKHR chooseSwapPresentMode(const VulkanDevice& device, const VkSurfaceKHR surface);
	VkExtent2D chooseSwapExtent(const VulkanDevice& device, const VkSurfaceKHR surface);
	void createImageViews(const VulkanDevice& device);

};

#endif // !VULKAN_SWAP_CHAIN_H

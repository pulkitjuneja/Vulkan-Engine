#pragma once

#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include "Window.h"
#include <vector>
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "EngineContext.h"
#include "VulkanHelpers.h"

struct VulkanSwapChain {

	VkSurfaceCapabilitiesKHR capabilities;
	VkSwapchainKHR vkSwapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass screenRenderPass;

	std::vector<VkImage> swapChainImages;
	AllocatedImage screenDepthBuffer;
	std::vector<VkImageView> swapChainImageViews; 
	VkImageView depthBufferImageView;
	std::vector<VkFramebuffer> frameBuffers;

	void initialize(const VulkanDevice& device, const VkSurfaceKHR surface);
	void release(const VulkanDevice& device);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VulkanDevice& device, const VkSurfaceKHR surface);
	VkPresentModeKHR chooseSwapPresentMode(const VulkanDevice& device, const VkSurfaceKHR surface);
	VkExtent2D chooseSwapExtent(const VulkanDevice& device, const VkSurfaceKHR surface);

	void createScreenRenderPass();
	void createFrameBuffers();
	void createImageViews(const VulkanDevice& device);

	uint32_t acquireNextImage(int currentFrameIndex);

};

#endif // !VULKAN_SWAP_CHAIN_H

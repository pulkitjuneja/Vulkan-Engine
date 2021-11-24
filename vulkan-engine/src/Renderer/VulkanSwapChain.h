#pragma once

#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include "Window.h"
#include <vector>
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "EngineContext.h"
#include "VulkanHelpers.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct VulkanSwapChain {

	VkSurfaceCapabilitiesKHR capabilities;
	VkSwapchainKHR vkSwapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass screenRenderPass;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VulkanCommandBuffer> screenCommandBUffers;

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
	void initScreenCommandBuffers();
	void createSemaphores();

	uint32_t acquireNextImage(int currentFrameIndex);

};

#endif // !VULKAN_SWAP_CHAIN_H

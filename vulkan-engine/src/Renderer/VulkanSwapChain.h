#pragma once

#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include "Window.h"
#include <vector>
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "EngineContext.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct VulkanSwapChain {

	VkSurfaceCapabilitiesKHR capabilities;
	VkSwapchainKHR vkSwapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VulkanCommandBuffer> screenCommandBUffers;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews; 
	std::vector<VkFramebuffer> frameBuffers;

	void initialize(const VulkanDevice& device, const VkSurfaceKHR surface);
	void release(const VulkanDevice& device);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VulkanDevice& device, const VkSurfaceKHR surface);
	VkPresentModeKHR chooseSwapPresentMode(const VulkanDevice& device, const VkSurfaceKHR surface);
	VkExtent2D chooseSwapExtent(const VulkanDevice& device, const VkSurfaceKHR surface);

	void initScreenCommandBuffers();
	void createFrameBuffers(const VulkanDevice& device, VkRenderPass& renderPass);
	void createImageViews(const VulkanDevice& device);
	void createSemaphores();


	uint32_t acquireNextImage(int currentFrameIndex);

	// NEed this because frame buffers need to be deleted before everything
	void destroySwapFrameBuffers(VulkanDevice& device);

};

#endif // !VULKAN_SWAP_CHAIN_H

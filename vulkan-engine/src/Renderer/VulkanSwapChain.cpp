#include "VulkanSwapChain.h"
#include "VulkanContext.h"

void VulkanSwapChain::initialize(const VulkanDevice& device, const VkSurfaceKHR surface)
{
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(device, surface);
	swapChainImageFormat = surfaceFormat.format;
	VkPresentModeKHR presentMode = chooseSwapPresentMode(device, surface);
	swapChainExtent = chooseSwapExtent(device, surface);

	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.presentMode = presentMode;
	createInfo.imageUsage = 16;

	QueueFamilyIndices indices = device.findQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// control sharing of swap chain across multiple queue families
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; 
		createInfo.pQueueFamilyIndices = nullptr; 
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	VkResult res = vkCreateSwapchainKHR(device.getLogicalDevice(), &createInfo, nullptr, &vkSwapChain);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device.getLogicalDevice(), vkSwapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device.getLogicalDevice(), vkSwapChain, &imageCount, swapChainImages.data());

	VkExtent3D depthImageExtent = {
		swapChainExtent.width, swapChainExtent.height, 1
	};

	screenDepthBuffer.create(VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent, 1, 1);
	// create depth buffer
	createImageViews(device);
	createScreenRenderPass();
	createFrameBuffers();

}

void VulkanSwapChain::release(const VulkanDevice& device)
{
	for (auto framebuffer : frameBuffers) {
		vkDestroyFramebuffer(device.getLogicalDevice(), framebuffer, nullptr);
	}

	vkDestroyRenderPass(device.getLogicalDevice(), screenRenderPass, nullptr);

	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device.getLogicalDevice(), imageView, nullptr);
	}

	VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;
	screenDepthBuffer.destroy();

	vkDestroySwapchainKHR(device.getLogicalDevice(), vkSwapChain, nullptr);
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const VulkanDevice& device, const VkSurfaceKHR surface)
{
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device.getPhysicalDevice(), surface, &formatCount, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device.getPhysicalDevice(), surface, &formatCount, formats.data());

	for (const auto& availableFormat : formats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return formats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const VulkanDevice& device, const VkSurfaceKHR surface)
{
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device.getPhysicalDevice(), surface, &presentModeCount, nullptr);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device.getPhysicalDevice(), surface, &presentModeCount,presentModes.data());

	for (const auto& availablePresentMode : presentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VulkanDevice& device, const VkSurfaceKHR surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getPhysicalDevice(), surface, &capabilities);

	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		GLFWwindow* window = EngineContext::get()->window->getNativeWindow();
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

}

void VulkanSwapChain::createScreenRenderPass()
{
	VkAttachmentDescription attachments[2];

	attachments[0].flags = 0;
	attachments[0].format = swapChainImageFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Depth attachment
	attachments[1].flags = 0;
	attachments[1].format = VK_FORMAT_D32_SFLOAT;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Create subpass for the attachment
	VkAttachmentReference attachmentRef{};
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachmentRef;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = &attachments[0];
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkDevice device = EngineContext::get()->vulkanContext->getDevice().getLogicalDevice();
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &screenRenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}

}

void VulkanSwapChain::createFrameBuffers()
{
	frameBuffers.resize(swapChainImageViews.size());
	for (int i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i],
			screenDepthBuffer.view
		};

		VkFramebufferCreateInfo framebufferInfo = vk::getFrameBufferCreateInfo(screenRenderPass, swapChainExtent, 2, &attachments[0]);

		VulkanDevice& device = EngineContext::get()->vulkanContext->getDevice();
		if (vkCreateFramebuffer(device.getLogicalDevice(), &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanSwapChain::createImageViews(const VulkanDevice& device)
{
	swapChainImageViews.resize(swapChainImages.size());

	for (int i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.image = swapChainImages[i];
		createInfo.format = swapChainImageFormat;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		if (vkCreateImageView(device.getLogicalDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}

	screenDepthBuffer.createView(VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);
}

uint32_t VulkanSwapChain::acquireNextImage(int currentFrameIndex)
{
	VulkanDevice& device = EngineContext::get()->vulkanContext->getDevice();
	auto frames = EC::get()->vulkanContext->frames;

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device.getLogicalDevice(), vkSwapChain, UINT64_MAX, frames[currentFrameIndex].imageAvailableSemaphore,
		VK_NULL_HANDLE, &imageIndex);
	return imageIndex;
}

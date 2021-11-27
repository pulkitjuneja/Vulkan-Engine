#include "VulkanContext.h"
#include "Uniforms.h"


void VulkanContext::initialize()
{
	instance.initialize();
	createSurface();
	device.initialize(instance, surface);
	creategraphicsPool();
	createVMAllocator();
	swapChain.initialize(device, surface);
	initializeFrameData();
}

void VulkanContext::release()
{
	releaseFrameData();
	swapChain.release(device);
	vmaDestroyAllocator(allocator);
	vkDestroyCommandPool(device.getLogicalDevice(), graphicsCommandPool, nullptr);
	device.release();
	vkDestroySurfaceKHR(instance.vkInstance,surface, nullptr);
	instance.release();
}

void VulkanContext::initializeFrameData()
{
	frames.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device.getLogicalDevice(), &semaphoreInfo, nullptr, &frames[i].imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(device.getLogicalDevice(), &semaphoreInfo, nullptr, &frames[i].renderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateFence(device.getLogicalDevice(), &fenceInfo, nullptr, &frames[i].inFlightFence) != VK_SUCCESS) {

			throw std::runtime_error("failed to create semaphores for a frame!");
		}
		frames[i].FrameCommandBuffer.initialize(graphicsCommandPool);
	}
}

void VulkanContext::releaseFrameData()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device.getLogicalDevice(), frames[i].renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device.getLogicalDevice(), frames[i].imageAvailableSemaphore, nullptr);
		vkDestroyFence(device.getLogicalDevice(), frames[i].inFlightFence, nullptr);
	}
}

void VulkanContext::createSurface()
{
	GLFWwindow* window = EngineContext::get()->window->getNativeWindow();
	if (glfwCreateWindowSurface(instance.vkInstance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::exception("Unable to create Surface");
	}
}

void VulkanContext::createVMAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = device.physicalDevice;
	allocatorInfo.device = device.logicalDevice;
	allocatorInfo.instance = instance.vkInstance;
	vmaCreateAllocator(&allocatorInfo, &allocator);
}

void VulkanContext::creategraphicsPool() 
{
	QueueFamilyIndices queueFamilyIndices = device.findQueueFamilies();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device.getLogicalDevice(), &poolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}



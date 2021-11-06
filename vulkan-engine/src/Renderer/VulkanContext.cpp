#include "VulkanContext.h"


void VulkanContext::initialize()
{
	instance.initialize();
	createSurface();
	device.initialize(instance, surface);
	swapChain.initialize(device, surface);
	creategraphicsPool();
}

void VulkanContext::release()
{
	vkDestroyCommandPool(device.getLogicalDevice(), graphicsCommandPool, nullptr);
	swapChain.release(device);
	//device.release();
	vkDestroySurfaceKHR(instance.vkInstance,surface, nullptr);
	instance.release();
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
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device.getLogicalDevice(), &poolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}



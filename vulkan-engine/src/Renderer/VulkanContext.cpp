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
	vkDestroyCommandPool(device.getLogicalDevice(), context.commandPool, nullptr);
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

	fenceInfo.flags = 0;
	vkCreateFence(device.getLogicalDevice(), &fenceInfo, nullptr, &context.fence);
}

void VulkanContext::releaseFrameData()
{
	vkDestroyFence(device.getLogicalDevice(), context.fence, nullptr);
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
		throw std::runtime_error("failed to create frame graphics command pool!");
	}

	poolInfo.flags = 0;
	vkCreateCommandPool(device.getLogicalDevice(), &poolInfo, nullptr, &context.commandPool);
}

void VulkanContext::immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VulkanCommandBuffer cmd;
	cmd.initialize(context.commandPool);
	cmd.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	function(cmd.commandBuffer);

	cmd.end();

	std::vector<VkSemaphore> waitSemaphores = {};
	std::vector<VkSemaphore> signalSemaphores = {};
	std::vector<VkCommandBuffer> cmds = { cmd.commandBuffer };
	VkSubmitInfo submitInfo = vkInit::getSubmitInfo(waitSemaphores, signalSemaphores, nullptr, cmds);

	vkQueueSubmit(device.queues.graphicsQueue, 1, &submitInfo, context.fence);

	vkWaitForFences(device.getLogicalDevice(), 1, &context.fence, true, 9999999999);
	vkResetFences(device.getLogicalDevice(), 1, &context.fence);
	
	//clear the command pool. This will free the command buffer too
	vkResetCommandPool(device.getLogicalDevice(), context.commandPool, 0);
}



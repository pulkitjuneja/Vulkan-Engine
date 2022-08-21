#include "VulkanContext.h"
#include "Uniforms.h"

namespace vk {
	void Context::initialize()
	{
		instance.initialize();
		createSurface();
		device.initialize(instance, surface);
		creategraphicsPool();
		createVMAllocator();
		swapChain.initialize(device, surface);
		initializeFrameData();
	}

	void Context::release()
	{
		releaseFrameData();
		swapChain.release(device);
		vmaDestroyAllocator(allocator);
		vkDestroyCommandPool(device.getLogicalDevice(), graphicsCommandPool, nullptr);
		vkDestroyCommandPool(device.getLogicalDevice(), context.commandPool, nullptr);
		device.release();
		vkDestroySurfaceKHR(instance.vkInstance, surface, nullptr);
		instance.release();
	}

	void Context::initializeFrameData()
	{
		//frames.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		imageAvailableSemaphores.forEach([&](VkSemaphore& sem) {
			if (vkCreateSemaphore(device.getLogicalDevice(), &semaphoreInfo, nullptr, &sem) != VK_SUCCESS) {
				Logger::logError("Error creating Image available semaphore");
			}
		});
		renderFinishSemaphores.forEach([&](VkSemaphore& sem) {
			if (vkCreateSemaphore(device.getLogicalDevice(), &semaphoreInfo, nullptr, &sem) != VK_SUCCESS) {
				Logger::logError("Error creating render semaphore");
			}
		});
		inFlightFences.forEach([&](VkFence& fence) {
			if (vkCreateFence(device.getLogicalDevice(), &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
				Logger::logError("Error creating fence");
			}
		});

		fenceInfo.flags = 0;
		vkCreateFence(device.getLogicalDevice(), &fenceInfo, nullptr, &context.fence);
	}

	void Context::releaseFrameData()
	{
		vkDestroyFence(device.getLogicalDevice(), context.fence, nullptr);
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device.getLogicalDevice(), renderFinishSemaphores[i], nullptr);
			vkDestroySemaphore(device.getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device.getLogicalDevice(), inFlightFences[i], nullptr);
		}
	}

	void Context::createSurface()
	{
		GLFWwindow* window = EngineContext::get()->window->getNativeWindow();
		if (glfwCreateWindowSurface(instance.vkInstance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::exception("Unable to create Surface");
		}
	}

	void Context::createVMAllocator()
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = device.physicalDevice;
		allocatorInfo.device = device.logicalDevice;
		allocatorInfo.instance = instance.vkInstance;
		vmaCreateAllocator(&allocatorInfo, &allocator);
	}

	void Context::creategraphicsPool()
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

	void Context::immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		vk::CommandBuffer cmd;
		cmd.create(context.commandPool);
		cmd.beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		function(cmd.handle);

		cmd.endRecording();

		std::vector<VkSemaphore> waitSemaphores = {};
		std::vector<VkSemaphore> signalSemaphores = {};
		std::vector<VkCommandBuffer> cmds = { cmd.handle };

		vk::CommandBuffer::submit(waitSemaphores, signalSemaphores, nullptr, cmds, context.fence);

		vkWaitForFences(device.getLogicalDevice(), 1, &context.fence, true, 9999999999);
		vkResetFences(device.getLogicalDevice(), 1, &context.fence);

		//clear the command pool. This will free the command buffer too
		// TODO : Shoudl I do this, what happens if multiple copy commands are happenign simultaneously 
		vkResetCommandPool(device.getLogicalDevice(), context.commandPool, 0);
	}
}



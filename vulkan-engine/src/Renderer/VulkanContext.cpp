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
	initDescriptors();
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
		frames[i].frameUniforms.createBuffer(sizeof(PerFrameUniforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &frameSetLayout;
		vkAllocateDescriptorSets(device.getLogicalDevice(), &allocInfo, &frames[i].frameDescriptor);

		VkDescriptorBufferInfo binfo;
		binfo.buffer = frames[i].frameUniforms.buffer;
		binfo.offset = 0;
		binfo.range = sizeof(PerFrameUniforms);
		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;
		setWrite.dstBinding = 0;
		setWrite.dstSet = frames[i].frameDescriptor;
		setWrite.descriptorCount = 1;
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setWrite.pBufferInfo = &binfo;

		vkUpdateDescriptorSets(device.getLogicalDevice(), 1, &setWrite, 0, nullptr);
	}
}

void VulkanContext::initDescriptors()
{
	VkDescriptorSetLayoutBinding frameBufferBinding = {};
	frameBufferBinding.binding = 0;
	frameBufferBinding.descriptorCount = 1;
	frameBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	frameBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo setInfo = {};
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	setInfo.bindingCount = 1;
	setInfo.flags = 0;
	setInfo.pBindings = &frameBufferBinding;

	vkCreateDescriptorSetLayout(device.getLogicalDevice(), &setInfo, nullptr, &frameSetLayout);

	std::vector<VkDescriptorPoolSize> sizes = {{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 }};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(device.getLogicalDevice(), &pool_info, nullptr, &descriptorPool);

}

void VulkanContext::releaseFrameData()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vmaDestroyBuffer(allocator, frames[i].frameUniforms.buffer, frames[i].frameUniforms.allocation);
		vkDestroySemaphore(device.getLogicalDevice(), frames[i].renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device.getLogicalDevice(), frames[i].imageAvailableSemaphore, nullptr);
		vkDestroyFence(device.getLogicalDevice(), frames[i].inFlightFence, nullptr);
	}
	vkDestroyDescriptorPool(device.getLogicalDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device.getLogicalDevice(), frameSetLayout, nullptr);
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



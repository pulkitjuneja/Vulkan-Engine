#pragma once

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "vk_mem_alloc.h"

#include "Core/Window.h"
#include <vector>
#include <functional>
#include "Core/Logger.h"
#include "VulkanDevice.h"
#include "Core/EngineContext.h"
#include "VulkanSwapChain.h"
#include "VulkanInstance.h"

//struct FrameData {
//	vk::Buffer frameUniforms;
//	vk::DescriptorSet frameDescriptor;
//
//	vk::Buffer objectBuffer;
//	vk::DescriptorSet objectDescriptor;
//};

struct immediateSubmitContext {
	VkFence fence;
	VkCommandPool commandPool;
};

namespace vk {
	class Context {
	protected:
		VulkanInstance instance;
		VulkanDevice device;
		VkSurfaceKHR surface;
		VulkanSwapChain swapChain;
		VkCommandPool graphicsCommandPool;

	public:
		VmaAllocator allocator;
		//std::vector<FrameData> frames;
		immediateSubmitContext context;
		VkDescriptorPool descriptorPool;
		PerFrameData<VkSemaphore> imageAvailableSemaphores;
		PerFrameData<VkSemaphore> renderFinishSemaphores;
		PerFrameData<VkFence> inFlightFences;

		Context() = default;
		void initialize();
		void release();

		VulkanInstance& getInstance() { return instance; }
		VulkanDevice& getDevice() { return device; }
		VkSurfaceKHR& getSurface() { return surface; }
		VulkanSwapChain& getSwapChain() { return swapChain; }
		VkCommandPool& getGraphicsCommandPool() { return graphicsCommandPool; }

		// main descriptor pool
		std::unordered_map<int, DescriptorSet> DescriptorSets;

		void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

		void initializeFrameData();
		void releaseFrameData();
		void createSurface();
		void createVMAllocator();
		void creategraphicsPool();
	};
}

#endif // !VULKAN_CONTEXT_H

#pragma once

#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Logger.h"
#include <optional>
#include <set>
#include "EngineContext.h"
#include "VulkanInstance.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct QueueHandles {
	VkQueue graphicsQueue;
	VkQueue presentQueue;
};

struct VulkanDevice {
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;
	VkSurfaceKHR surfaceRef;

	const std::vector<const char*> requestedDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	QueueHandles queues;

	void initialize(const VulkanInstance instance,
		const VkSurfaceKHR currentSurface);
	void release();

	void initPhysicalDevice(VkInstance instance);
	void initLogicalDevice(bool enableValidationLayers);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device = nullptr) const;
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool querySwapChainSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);

	VkPhysicalDevice getPhysicalDevice() const;
	VkDevice getLogicalDevice() const;
};

#endif // !VULKAN_DEVICE_H

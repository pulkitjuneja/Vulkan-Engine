#pragma once

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "Window.h"
#include <vector>
#include "Logger.h"
#include "VulkanDevice.h"
#include "EngineContext.h"
#include "VulkanSwapChain.h"

class VulkanContext {
protected:
	VkInstance instance;
	VulkanDevice devices;
	VkSurfaceKHR surface;
	VulkanSwapChain swapChain;

	VkDebugUtilsMessengerEXT debugMessenger;
	bool enableValidationLayers;

public:
	VulkanContext() = default;
	void initialize();
	void release();


	bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);
	std::vector<const char*> getRequiredExtensions(bool);
	void createSurface();

	// debug messenger setup
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);
	void setupDebugMessenger();
	void destroyDebugMessenger();
};

#endif // !VULKAN_CONTEXT_H

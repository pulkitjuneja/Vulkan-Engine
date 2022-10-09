#pragma once

#ifndef VULKAN_INSTANCE
#define VULKAN_INSTANCE

#include "Core/Window.h"
#include <vector>
#include "Core/Logger.h"

struct VulkanInstance {
	VkInstance vkInstance;
	VkDebugUtilsMessengerEXT debugMessenger;
	bool enableValidationLayers;

	void initialize();
	void release();

	std::vector<const char*> getRequiredExtensions(bool);
	bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);

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

#endif // !VULKAN_INSTANCE

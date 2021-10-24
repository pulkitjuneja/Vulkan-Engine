#include "VulkanDevice.h"

void VulkanDevice::initialize(const VulkanInstance instance, const VkSurfaceKHR currentSurface)
{
	surfaceRef = currentSurface;
	initPhysicalDevice(instance.vkInstance);
	initLogicalDevice(instance.enableValidationLayers);
}

void VulkanDevice::initPhysicalDevice(VkInstance instance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::exception("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == nullptr) {
		throw std::exception("Unable to find suitable physical device");
	}
}

void VulkanDevice::initLogicalDevice(bool enableValidationLayers)
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	uint32_t uniqueQueueFamilies [] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requestedDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = requestedDeviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::exception("Error initializing logical device");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &queues.graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &queues.presentQueue);

}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const
{
	QueueFamilyIndices indices;

	device = device == nullptr ? physicalDevice : device;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surfaceRef, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}
	}

	return indices;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(requestedDeviceExtensions.begin(), 
		requestedDeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
		
	return requiredExtensions.empty();

}

bool VulkanDevice::querySwapChainSupport(VkPhysicalDevice device)
{
	VkSurfaceCapabilitiesKHR sc{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surfaceRef, &sc);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceRef, &formatCount, nullptr);

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceRef, &presentModeCount, nullptr);

	return formatCount > 0 && presentModeCount > 0;
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device)
{	
	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(device, &props);

	QueueFamilyIndices& indices = findQueueFamilies(device);
	// check for required device feature support here;
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	return indices.isComplete() && extensionsSupported && querySwapChainSupport(device);
}

VkPhysicalDevice VulkanDevice::getPhysicalDevice() const
{
	return physicalDevice;
}

VkDevice VulkanDevice::getLogicalDevice() const
{
	return logicalDevice;
}

void VulkanDevice::release()
{
	vkDestroyDevice(logicalDevice, nullptr);
}

#pragma once


#ifndef FORWARDRENDERER
#define FORWARDRENDERER

#include "System.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class ForwardRenderer : public ISystem {
protected:
	VkDescriptorSetLayout frameSetLayout;
	VkDescriptorPool descriptorPool;
public:
	void startup();
	void update(float deltaTime);
	void shutdown();
};

#endif // !FORWARDRENDERER

#pragma once


#ifndef FORWARDRENDERER
#define FORWARDRENDERER

#include "System.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "SceneRenderer.h"

class ForwardRenderer : public ISystem {
protected:
	VkDescriptorSetLayout frameSetLayout;
	VkDescriptorPool descriptorPool;
	SceneRenderer sceneRenderer;

	int currentFrame = 0;
public:
	void startup();
	void update(float deltaTime);
	void shutdown();

	void initDescriptorSets();
};

#endif // !FORWARDRENDERER

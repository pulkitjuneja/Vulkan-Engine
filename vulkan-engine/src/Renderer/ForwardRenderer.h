#pragma once


#ifndef FORWARDRENDERER
#define FORWARDRENDERER

#include "System.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "SceneRenderer.h"

#define MAX_OBJECT_COUNT 1024

class ForwardRenderer : public ISystem {
protected:
	VkDescriptorSetLayout frameSetLayout;
	VkDescriptorSetLayout objectUniformLayout;

	SceneRenderer sceneRenderer;

	int currentFrame = 0;
public:
	void startup();
	void update(float deltaTime);
	void shutdown();

	void initDescriptorSets();
};

#endif // !FORWARDRENDERER

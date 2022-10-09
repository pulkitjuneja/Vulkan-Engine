#pragma once


#ifndef FORWARDRENDERER
#define FORWARDRENDERER

#include "Core/System.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "SceneRenderer.h"
#include "VulkanHelpers.h"

#define MAX_OBJECT_COUNT 1024

class ForwardRenderer : public ISystem {
protected:
	vk::DescriptorSetLayout frameSetLayout;
	vk::DescriptorSetLayout objectUniformLayout;
	PerFrameData<vk::CommandBuffer> frameCommandBuffers;

	// Global Uniform buffer and descriptorset
	PerFrameData<vk::Buffer> frameUniforms;
	PerFrameData<vk::DescriptorSet> frameDescriptors;

	// Per entity data buffer and descriptor set
	PerFrameData<vk::Buffer> objectBuffers;
	PerFrameData<vk::DescriptorSet> objectDescriptorSet;

	SceneRenderer sceneRenderer;

	int currentFrame = 0;
public:
	void startup();
	void update(float deltaTime);
	void shutdown();

	void initDescriptorSets();
};

#endif // !FORWARDRENDERER

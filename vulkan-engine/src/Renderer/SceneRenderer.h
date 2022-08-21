#pragma once

#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include "Scene/Scene.h"
#include "Uniforms.h"
#include "EngineContext.h"
#include "VulkanContext.h"
#include "System.h"

struct FrameData {
	vk::CommandBuffer cmd;
	vk::Buffer frameUniforms;
	vk::Buffer objectUniforms;
	vk::DescriptorSet frameDescriptorSet;
	vk::DescriptorSet objectDescriptorSet;
	VkFence inFlightfence;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	int frameIndex;
};

class SceneRenderer {
public:
	void renderScene(FrameData& frame,
		bool passBaseMaterialProperties = false);

	void updateSceneUniforms(FrameData& frame);
};

#endif // !SCENERENDERER_H

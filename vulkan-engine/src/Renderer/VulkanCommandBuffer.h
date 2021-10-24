#pragma once

#ifndef VULKAN_COMMAND_POOL_H
#define VULKAN_COMMAND_POOL_H

#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "EngineContext.h"

struct VulkanCommandBuffer {

	VkCommandBuffer commandBuffer;

	void initialize(VkCommandPool& commandPool);

	void begin();
	void beginRenderPass(VkRenderPass& renderPass, VkFramebuffer& frameBuffer, VkExtent2D& extents);
	void bindPipeline(VkPipeline& graphicsPipeline);
	void draw(int vertexCount);
	void endRenderPass();
	void end();

};

#endif // !VULKAN_COMMAND_POOL_H

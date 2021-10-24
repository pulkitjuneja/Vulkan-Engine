#pragma once

#ifndef STANDARD_PIPELINE
#define STANDARD_PIPELINE

#include <string>
#include <fstream>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TODO: rework pipeline creation to be more flexible
class StandardPipeline {
protected:
	VkPipelineShaderStageCreateInfo shaderStages[2];
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	VkViewport viewport {};
	VkRect2D scissor{};
	VkPipelineViewportStateCreateInfo viewportState{};
	VkPipelineRasterizationStateCreateInfo rasterizer{};

	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDescription> subPasses;
	std::vector<VkAttachmentReference> attachmentRefs;

	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
public:
	std::vector<char> readFile(std::string filePath);

	// TODO: Parameterize these functions to customize pipeline
	void createInputAssemblyInfo();
	void createViewPortInfo();
	void createRasterizerInfo();
	void createShaderStage(std::string vertPath, std::string fragPath);
	void addAttachment();
	void addSubPass();
	VkPipeline build();
	void release();

	//Temp function to build pipeline with default settings
	void createDefault();

	VkPipeline& getPipeline() { return graphicsPipeline; }
	VkRenderPass& getRenderPass() { return renderPass; }
};

#endif // !STANDARD_PIPELINE

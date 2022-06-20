#pragma once

#ifndef PIPELINE_BUILDER
#define PIPELINE_BUILDER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include <vector>
#include "Renderer/VulkanHelpers.h"

class PipelineBuilder {
public:

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
	VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
	VkViewport viewport {};
	VkRect2D scissor {};
	VkPipelineRasterizationStateCreateInfo rasterizer {};
	VkPipelineColorBlendAttachmentState colorBlendAttachment {};
	VkPipelineMultisampleStateCreateInfo multisampling{};
	VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
	VkPipelineDepthStencilStateCreateInfo depthStencil{};

	std::vector<VkShaderModule> shaderModules;
	VkVertexInputBindingDescription bindingDescription {};
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};

	GraphicsPipeline Cache;

	PipelineBuilder& setVertexInputStateInfo();
	PipelineBuilder& setPipelineInputAssemblyStateInfo(VkPrimitiveTopology topology);
	//TODO: PArameterize it further to take an array of modules and stage flags. Move loader to Resource manager
	PipelineBuilder& setShaderStagesInfo(std::string&& vertPath, std::string&& fragPath);
	PipelineBuilder& setViewPortInfo(VkExtent2D& extents);
	PipelineBuilder& setScissor(VkOffset2D offset, VkExtent2D& extents);
	PipelineBuilder& setRasterizerInfo(VkPolygonMode polygonMode);
	PipelineBuilder& setMultiSamplingInfo();
	PipelineBuilder& setPipelineLayout(VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout PerObjectLayout);
	PipelineBuilder& DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);
	GraphicsPipeline build_pipeline(VkRenderPass pass);

	std::vector<char> readFile(std::string filePath);
};

#endif // !PIPELINE_BUILDER

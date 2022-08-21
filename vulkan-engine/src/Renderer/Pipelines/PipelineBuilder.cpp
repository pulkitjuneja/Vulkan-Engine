#include "PipelineBuilder.h"
#include "Renderer/Vertex.h"
#include <fstream>
#include "EngineContext.h"
#include "Renderer/VulkanContext.h"
#include "Renderer/Uniforms.h"

vk::GraphicsPipeline PipelineBuilder::build_pipeline(VkRenderPass pass)
{
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkDevice device = EngineContext::get()->vulkanContext->getDevice().getLogicalDevice();

	VkDescriptorSetLayout layouts[2] = { Cache.GlobalUniformLayout.handle, Cache.PerObjectLayout.handle};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 2;
	pipelineLayoutInfo.pSetLayouts = layouts;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &Cache.pipelineLayout) != VK_SUCCESS) {
		Logger::logError("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = &shaderStages[0];
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = Cache.pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &Cache.pipeline) != VK_SUCCESS) {
		Logger::logError("failed to create graphics pipeline!");
	}

	for (VkShaderModule shaderModule : shaderModules) {
		vkDestroyShaderModule(device, shaderModule, nullptr);
	}

	return Cache;
}

PipelineBuilder& PipelineBuilder::setVertexInputStateInfo()
{
	bindingDescription = Vertex::getBindingDescription();
	attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	return *this;
}

PipelineBuilder& PipelineBuilder::setPipelineInputAssemblyStateInfo(VkPrimitiveTopology topology)
{
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = nullptr;
	inputAssembly.topology = topology;
	//Todo: when to use this
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	return *this;
}

PipelineBuilder& PipelineBuilder::setShaderStagesInfo(std::string&& vertPath, std::string&& fragPath)
{
	shaderStages.resize(2);
	shaderModules.resize(2);

	auto vertShaderCode = readFile(vertPath);
	auto fragShaderCode = readFile(fragPath);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = vertShaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

	VkDevice logicalDevice = EC::get()->vulkanContext->getDevice().getLogicalDevice();

	if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModules[0]) != VK_SUCCESS) {
		Logger::logError("failed to create vertex shader module! for" + vertPath);
	}

	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fragShaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

	if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModules[1]) != VK_SUCCESS) {
		Logger::logError("failed to create fragment shader module for" + fragPath);
	}

	// vertex shader stage
	shaderStages[0] = {};
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = shaderModules[0];
	shaderStages[0].pName = "main";

	// fragment shader stage
	shaderStages[1] = {};
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = shaderModules[1];
	shaderStages[1].pName = "main";

	return *this;

}

PipelineBuilder& PipelineBuilder::setViewPortInfo(VkExtent2D& extents)
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extents.width;
	viewport.height = (float)extents.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	return *this;
}

PipelineBuilder& PipelineBuilder::setScissor(VkOffset2D offset, VkExtent2D& extents)
{
	scissor.offset = offset;
	scissor.extent = extents;

	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerInfo(VkPolygonMode polygonMode)
{
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	return *this;
}

PipelineBuilder& PipelineBuilder::setMultiSamplingInfo()
{
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Uncomment and see;
	//multisampling.minSampleShading = 1.0f;
	//multisampling.pSampleMask = nullptr;
	//multisampling.alphaToCoverageEnable = VK_FALSE;
	//multisampling.alphaToOneEnable = VK_FALSE;

	return *this;
}

PipelineBuilder& PipelineBuilder::setPipelineLayout(vk::DescriptorSetLayout globalLayout, vk::DescriptorSetLayout PerObjectLayout)
{
	Cache.GlobalUniformLayout = globalLayout;
	Cache.PerObjectLayout = PerObjectLayout;

	return *this;
}

PipelineBuilder& PipelineBuilder::DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
{
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.pNext = nullptr;

	depthStencil.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
	depthStencil.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;

	return *this;
}

std::vector<char> PipelineBuilder::readFile(std::string filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}


#pragma once

#ifndef BASIC_PIPELINE
#define BASIC_PIPELINE

#include <string>
#include <fstream>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class BasicPipeline {
protected:
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;

public:
	std::vector<char> readFile(std::string filePath);

	void build(std::string&& vertPath, std::string&& fragPath, VkRenderPass& renderPass);
	void release();

	VkPipeline& getPipeline() { return graphicsPipeline; }
	VkPipelineLayout& getPipelinelayout() { return pipelineLayout; }
};


#endif
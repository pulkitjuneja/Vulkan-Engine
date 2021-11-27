#include "ForwardRenderer.h"
#include "Renderer/VulkanHelpers.h"
#include "EngineContext.h"
#include "VulkanContext.h"
#include "ResourceManagement/ResourceManager.h"
#include <vector>
#include "Uniforms.h"
#include "Pipelines/PipelineBuilder.h"


void ForwardRenderer::startup()
{
	initDescriptorSets();

	std::vector<VkDescriptorSetLayout> layouts;
	layouts.push_back(frameSetLayout);

	PipelineBuilder builder;
	GraphicsPipeline pipeline = builder.setVertexInputStateInfo().setPipelineInputAssemblyStateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST).
		setShaderStagesInfo("Assets/shaders/triangleVert.spv", "Assets/shaders/triangleFrag.spv").
		setViewPortInfo(EC::get()->vulkanContext->getSwapChain().swapChainExtent).
		setScissor({ 0,0 }, EC::get()->vulkanContext->getSwapChain().swapChainExtent).
		setRasterizerInfo(VK_POLYGON_MODE_FILL).
		setMultiSamplingInfo().
		setPipelineLayout(layouts).
		DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL).
		build_pipeline(EC::get()->vulkanContext->getSwapChain().screenRenderPass);

	EC::get()->resourceManager->savePipeline("BasePipeine", pipeline);
}

void ForwardRenderer::shutdown()
{
	auto allocator = EC::get()->vulkanContext->allocator;
	auto& frames = EC::get()->vulkanContext->frames;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vmaDestroyBuffer(allocator, frames[i].frameUniforms.buffer, frames[i].frameUniforms.allocation);
	}
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, frameSetLayout, nullptr);
}

void ForwardRenderer::initDescriptorSets()
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	auto& frames = EC::get()->vulkanContext->frames;

	VkDescriptorSetLayoutBinding frameBufferBinding = vkInit::getDescripterLayoutBindingInfo(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_SHADER_STAGE_VERTEX_BIT, 0);

	VkDescriptorSetLayoutCreateInfo setInfo = {};
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	setInfo.bindingCount = 1;
	setInfo.flags = 0;
	setInfo.pBindings = &frameBufferBinding;

	vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &frameSetLayout);

	std::vector<VkDescriptorPoolSize> sizes = { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 } };

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkBufferCreateInfo frameUniformsBufferInfo = vkInit::getBufferCreateinfo(sizeof(PerFrameUniforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;
		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		if (vmaCreateBuffer(alloc, &frameUniformsBufferInfo, &vmaallocInfo, &frames[i].frameUniforms.buffer,
			&frames[i].frameUniforms.allocation, nullptr) != VK_SUCCESS) {
			Logger::logError("failed to create Frame uniform buffers");
		}

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &frameSetLayout;
		vkAllocateDescriptorSets(device, &allocInfo, &frames[i].frameDescriptor);

		VkDescriptorBufferInfo binfo;
		binfo.buffer = frames[i].frameUniforms.buffer;
		binfo.offset = 0;
		binfo.range = sizeof(PerFrameUniforms);

		VkWriteDescriptorSet setWrite = vkInit::writeDescriptorSet(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, frames[i].frameDescriptor,
			&binfo, 0);

		vkUpdateDescriptorSets(device, 1, &setWrite, 0, nullptr);
	}
}

void ForwardRenderer::update(float deltaTime) {

	sceneRenderer.renderScene(currentFrame);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

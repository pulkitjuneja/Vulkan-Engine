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
	layouts.push_back(objectUniformLayout);

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
		vmaDestroyBuffer(allocator, frames[i].objectBuffer.buffer, frames[i].objectBuffer.allocation);
		vmaDestroyBuffer(allocator, frames[i].frameUniforms.buffer, frames[i].frameUniforms.allocation);
	}
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, frameSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, objectUniformLayout, nullptr);
}

void ForwardRenderer::initDescriptorSets()
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	auto& frames = EC::get()->vulkanContext->frames;

	// Set layout for uniform buffer
	VkDescriptorSetLayoutBinding frameBufferBinding = vkInit::getDescripterLayoutBindingInfo(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	VkDescriptorSetLayoutCreateInfo setInfo = {};
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	setInfo.bindingCount = 1;
	setInfo.flags = 0;
	setInfo.pBindings = &frameBufferBinding;

	vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &frameSetLayout);

	// Set layout for object buffer
	VkDescriptorSetLayoutBinding objectBind = vkInit::getDescripterLayoutBindingInfo(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
		VK_SHADER_STAGE_VERTEX_BIT, 0);
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	setInfo.bindingCount = 1;
	setInfo.flags = 0;
	setInfo.pBindings = &objectBind;

	vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &objectUniformLayout);

	std::vector<VkDescriptorPoolSize> sizes = { 
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 }, 
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 } 
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {


		VkBufferCreateInfo frameUniformsBufferInfo = vkInit::getBufferCreateinfo(sizeof(PerFrameUniforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		VkBufferCreateInfo objectUniformsBufferInfo = vkInit::getBufferCreateinfo(sizeof(PerObjectUniforms) * MAX_OBJECT_COUNT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

		VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;
		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		vmaCreateBuffer(alloc, &frameUniformsBufferInfo, &vmaallocInfo, &frames[i].frameUniforms.buffer,
			&frames[i].frameUniforms.allocation, nullptr);

		vmaCreateBuffer(alloc, &objectUniformsBufferInfo, &vmaallocInfo, &frames[i].objectBuffer.buffer,
			&frames[i].objectBuffer.allocation, nullptr);

		// Allocate frame uniform descriptor set
		VkDescriptorSetAllocateInfo allocInfo = vkInit::getDescriptorAllocInfo(descriptorPool, &frameSetLayout, 1);
		vkAllocateDescriptorSets(device, &allocInfo, &frames[i].frameDescriptor);

		// Allocate Object data descriptor Set
		allocInfo = vkInit::getDescriptorAllocInfo(descriptorPool, &objectUniformLayout, 1);
		vkAllocateDescriptorSets(device, &allocInfo, &frames[i].objectDescriptor);

		VkDescriptorBufferInfo frameBUfferInfo{};
		frameBUfferInfo.buffer = frames[i].frameUniforms.buffer;
		frameBUfferInfo.offset = 0;
		frameBUfferInfo.range = sizeof(PerFrameUniforms);

		VkDescriptorBufferInfo ObjectBufferInfo{};
		ObjectBufferInfo.buffer = frames[i].objectBuffer.buffer;
		ObjectBufferInfo.offset = 0;
		ObjectBufferInfo.range = sizeof(PerObjectUniforms) * MAX_OBJECT_COUNT;

		VkWriteDescriptorSet setWrite = vkInit::writeDescriptorSet(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, frames[i].frameDescriptor,
			&frameBUfferInfo, 0);

		VkWriteDescriptorSet objectDescriptorWrite = vkInit::writeDescriptorSet(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
			frames[i].objectDescriptor, &ObjectBufferInfo, 0);

		VkWriteDescriptorSet setWrites[] = { setWrite, objectDescriptorWrite };

		vkUpdateDescriptorSets(device, 2, setWrites, 0, nullptr);
	}
}

void ForwardRenderer::update(float deltaTime) {

	sceneRenderer.updateSceneUniforms(currentFrame);
	sceneRenderer.renderScene(currentFrame);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

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

	//std::vector<VkDescriptorSetLayout> layouts;
	//layouts.push_back(frameSetLayout);
	frameCommandBuffers.forEach([&](vk::CommandBuffer& cmd) {
		auto context = EC::get()->vulkanContext;
		cmd.create(context->getGraphicsCommandPool());
		std::cout << "x";
	});

	PipelineBuilder builder;
	vk::GraphicsPipeline pipeline = builder.setVertexInputStateInfo().setPipelineInputAssemblyStateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST).
		setShaderStagesInfo("Assets/shaders/triangleVert.spv", "Assets/shaders/triangleFrag.spv").
		setViewPortInfo(EC::get()->vulkanContext->getSwapChain().swapChainExtent).
		setScissor({ 0,0 }, EC::get()->vulkanContext->getSwapChain().swapChainExtent).
		setRasterizerInfo(VK_POLYGON_MODE_FILL).
		setMultiSamplingInfo().
		setPipelineLayout(frameSetLayout, objectUniformLayout).
		DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL).
		build_pipeline(EC::get()->vulkanContext->getSwapChain().screenRenderPass);

	EC::get()->resourceManager->savePipeline("BasePipeine", pipeline);
}

void ForwardRenderer::shutdown()
{
	auto allocator = EC::get()->vulkanContext->allocator;
	//auto& frames = EC::get()->vulkanContext->frames;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		frameUniforms[i].destroy();
		objectBuffers[i].destroy();
	}
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	vkDestroyDescriptorPool(device, EC::get()->vulkanContext->descriptorPool, nullptr);
	frameSetLayout.destroy();
	objectUniformLayout.destroy();
}

void ForwardRenderer::initDescriptorSets()
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();

	frameSetLayout.create({ 
		{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},
		{1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}
	});

	objectUniformLayout.create({
		{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
	});

	std::vector<VkDescriptorPoolSize> sizes = { 
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100}, 
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 100;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	VkResult res = vkCreateDescriptorPool(device, &pool_info, nullptr, &EC::get()->vulkanContext->descriptorPool);

	frameUniforms.forEach([&](vk::Buffer& buffer) { buffer.create(sizeof(PerFrameUniforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU); });
	objectBuffers.forEach([&](vk::Buffer& buffer) { buffer.create(sizeof(PerObjectUniforms) * MAX_OBJECT_COUNT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU); });


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		frameSetLayout.setBuffer(0, &frameUniforms[i], 0, frameUniforms[i].bufferSize);
		frameSetLayout.setBuffer(1, &objectBuffers[i], 0, objectBuffers[i].bufferSize);
		frameSetLayout.getDescriptorSet(frameDescriptors[i]);
	}
}

void ForwardRenderer::update(float deltaTime) {

	auto context = EC::get()->vulkanContext;

	FrameData frame = {frameCommandBuffers[currentFrame],
	frameUniforms[currentFrame],
	objectBuffers[currentFrame],
	frameDescriptors[currentFrame],
	objectDescriptorSet[currentFrame],
	context->inFlightFences[currentFrame],
	context->imageAvailableSemaphores[currentFrame],
	context->renderFinishSemaphores[currentFrame],
	currentFrame};

	sceneRenderer.updateSceneUniforms(frame);
	sceneRenderer.renderScene(frame);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"
#include "Constants.h"
#include "Renderer/Pipelines/PipelineBuilder.h"


Engine::Engine() : mainAllocator(1024 * 1024 * 800) {}


void Engine::initScene()
{
	Mesh* monkeyMesh = resourceManager->loadMesh("Assets/Meshes/BlenderMonkey.obj");
	scene.setMainCamera(glm::vec3(0, 0, -2.0f), glm::vec3(0, 90, 0), 80.0f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 200.0f);
	scene.createEntity("triangle", monkeyMesh).pipeline = &pipeline;
}

void Engine::renderLoop()
{
	renderer.renderScene(scene, currentFrame);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::start()
{
	EC::get()->mainAllocator = &mainAllocator;

	window = std::make_unique<Window>(1366, 768, "Vulkan Engine");
	resourceManager = Mem::Allocate<ResourceManager>();

	EC::get()->window = window.get();
	EC::get()->resourceManager = resourceManager;

	isEngineRunning = true;

	try {
		graphicsContext = std::make_unique<VulkanContext>();
		EC::get()->vulkanContext = graphicsContext.get();
		graphicsContext->initialize();
		//pipeline.build("Assets/shaders/triangleVert.spv", "Assets/shaders/triangleFrag.spv", 
		//	EngineContext::get()->vulkanContext->getSwapChain().screenRenderPass);
		std::vector<VkDescriptorSetLayout> layouts;
		layouts.push_back(EC::get()->vulkanContext->frameSetLayout);

		PipelineBuilder builder;
		pipeline = builder.setVertexInputStateInfo().setPipelineInputAssemblyStateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST).
			setShaderStagesInfo("Assets/shaders/triangleVert.spv", "Assets/shaders/triangleFrag.spv").
			setViewPortInfo(EC::get()->vulkanContext->getSwapChain().swapChainExtent).
			setScissor({ 0,0 }, EC::get()->vulkanContext->getSwapChain().swapChainExtent).
			setRasterizerInfo(VK_POLYGON_MODE_FILL).
			setMultiSamplingInfo().
			setPipelineLayout(layouts).
			DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL).
			build_pipeline(EC::get()->vulkanContext->getSwapChain().screenRenderPass);
		//BasicPipeline pipeline;
		//pipeline.build("Assets/shaders/triangleVert.spv", "Assets/shaders/triangleFrag.spv",
		//		EngineContext::get()->vulkanContext->getSwapChain().screenRenderPass);

	}
	catch (const std::exception& e) {
		Logger::logError(e.what());
		isEngineRunning = false;
	}


	initScene();

	while (isEngineRunning) {
		if (glfwWindowShouldClose(window->getNativeWindow())) {
			isEngineRunning = false;
		}
		window->display();
		renderLoop();
	}
	vkDeviceWaitIdle(graphicsContext->getDevice().logicalDevice);
}

Engine::~Engine()
{	
	scene.release();

	VkDevice device = EngineContext::get()->vulkanContext->getDevice().getLogicalDevice();

	vkDestroyPipeline(device, pipeline.pipeline , nullptr);
	vkDestroyPipelineLayout(device, pipeline.pipelineLayout, nullptr);

	graphicsContext->release();
	window->shutdown();
}

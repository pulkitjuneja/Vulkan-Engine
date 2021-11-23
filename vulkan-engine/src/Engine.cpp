#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"


Engine::Engine() : mainAllocator(1024 * 1024 * 800) {}


void Engine::initScene()
{
	//std::vector<Vertex> triangleVertices;
	//triangleVertices.resize(4);

	////vertex positions
	//triangleVertices[0].position = { -0.5f, -0.5f, 0.0f };
	//triangleVertices[1].position = { 0.5f, -0.5f, 0.0f };
	//triangleVertices[2].position = { 0.5f, 0.5f, 0.0f };
	//triangleVertices[3].position = { -0.5f, 0.5f, 0.0f };

	////vertex colors, all green
	//triangleVertices[0].color = { 1.f, 0.0f, 0.0f }; 
	//triangleVertices[1].color = { 0.f, 1.f, 0.0f }; 
	//triangleVertices[2].color = { 0.f, 0.0f, 1.0f }; 
	//triangleVertices[3].color = { 0.f, 0.0f, 1.0f };

	//std::vector<uint16_t> indices = {
	//	0, 1, 2, 2, 3, 0
	//};
	Mesh* monkeyMesh = resourceManager->loadMesh("Assets/Meshes/BlenderMonkey.obj");
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
		pipeline.build("Assets/shaders/triangleVert.spv", "Assets/shaders/triangleFrag.spv", 
			EngineContext::get()->vulkanContext->getSwapChain().screenRenderPass);

		// See if this can be reorderd
		graphicsContext->getSwapChain().createSemaphores();
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
	pipeline.release();
	graphicsContext->release();
	window->shutdown();
}

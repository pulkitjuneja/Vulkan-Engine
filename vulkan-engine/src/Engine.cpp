#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"

void Engine::initScene()
{
	std::vector<Vertex> triangleVertices;
	triangleVertices.resize(3);

	//vertex positions
	triangleVertices[0].position = { 0.5f, 0.5f, 0.0f };
	triangleVertices[1].position = { -0.5f, 0.5f, 0.0f };
	triangleVertices[2].position = { 0.f,-0.5f, 0.0f };

	//vertex colors, all green
	triangleVertices[0].color = { 1.f, 0.0f, 0.0f }; //pure green
	triangleVertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
	triangleVertices[2].color = { 0.f, 0.0f, 1.0f }; //pure green

	Mesh* triangleMesh = new Mesh(std::move(triangleVertices));
	scene.createEntity("triangle", triangleMesh).pipeline = &pipeline;
}

void Engine::renderLoop()
{
	renderer.renderScene(scene, currentFrame);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::start()
{
	window = std::make_unique<Window>(1366, 768, "Vulkan Engine");
	EngineContext::get()->window = window.get();

	isEngineRunning = true;

	try {
		graphicsContext = std::make_unique<VulkanContext>();
		EngineContext::get()->vulkanContext = graphicsContext.get();
		graphicsContext->initialize();
		pipeline.build("shaders/triangleVert.spv", "shaders/triangleFrag.spv");
		graphicsContext->getSwapChain().createFrameBuffers(graphicsContext->getDevice(),
			pipeline.getRenderPass());
		graphicsContext->getSwapChain().initScreenCommandBuffers();

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
	graphicsContext->getSwapChain().destroySwapFrameBuffers(graphicsContext->getDevice());
	pipeline.release();
	graphicsContext->release();
	window->shutdown();
}

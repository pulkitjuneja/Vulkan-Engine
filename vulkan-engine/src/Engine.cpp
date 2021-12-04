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
	scene.createDirectionalLight(glm::vec4(-1, -1, 0, 0), glm::vec4(1, 1, 1, 1.0), 5);

	resourceManager->loadTexture("Assets/Textures/crate_1.jpg");

	Entity& monke = scene.createEntity("Monke", monkeyMesh);
	monke.pipeline = &resourceManager->getPipeline("BasePipeine");
	monke.transform.setPosition(glm::vec3(3, 0, 3));

	Entity& monke2 = scene.createEntity("triangle", monkeyMesh);
	monke2.pipeline = &resourceManager->getPipeline("BasePipeine");
	monke2.transform.setPosition(glm::vec3(0, 0, 2));

	Entity& monke3 = scene.createEntity("Monke3", monkeyMesh);
	monke3.pipeline = &resourceManager->getPipeline("BasePipeine");
	monke3.transform.setPosition(glm::vec3(-3, 0, 3));
}

void Engine::renderLoop()
{
	//renderer.renderScene(scene, currentFrame);
	renderer.update(0);
}

void Engine::start()
{
	EC::get()->mainAllocator = &mainAllocator;

	window = std::make_unique<Window>(1366, 768, "Vulkan Engine");
	resourceManager = Mem::Allocate<ResourceManager>();

	EC::get()->window = window.get();
	EC::get()->resourceManager = resourceManager;
	EC::get()->scene = &scene;

	isEngineRunning = true;

	try {
		graphicsContext = std::make_unique<VulkanContext>();
		EC::get()->vulkanContext = graphicsContext.get();
		graphicsContext->initialize();
	}
	catch (const std::exception& e) {
		Logger::logError(e.what());
		isEngineRunning = false;
	}

	renderer.startup();
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
	renderer.shutdown();
	scene.release();
	resourceManager->release();
	graphicsContext->release();
	window->shutdown();
}

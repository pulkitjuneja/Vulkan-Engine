#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"
#include "Constants.h"
#include "Renderer/Pipelines/PipelineBuilder.h"


Engine::Engine() : mainAllocator(1024 * 1024 * 800),
resourceManager(&mainAllocator) {}


void Engine::initScene()
{
	// Todo : Move default resource initialization to engine specific function
	resourceManager.loadTexture("Assets/Textures/Default.jpg");

	Mesh* cubeMesh = resourceManager.loadMesh("Assets/Meshes/sponza/sponza.obj");
	Mesh* monkeyMesh = resourceManager.loadMesh("Assets/Meshes/BlenderMonkey.obj");
	//Mesh* cubeMesh = resourceManager->loadMesh("Assets/Meshes/Crate/Crate1.obj");
	scene.setMainCamera(glm::vec3(0, 0, -2.0f), glm::vec3(0, 90, 0), 80.0f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 200.0f);
	scene.createDirectionalLight(glm::vec4(-1, -1, 0, 0), glm::vec4(1, 1, 0, 1.0), 5);

	Entity& monke = scene.createEntity("Monke", monkeyMesh);
	monke.transform.setPosition(glm::vec3(3, 0, 3));

	Entity& cube = scene.createEntity("Cube", cubeMesh);
	cube.transform.setPosition(glm::vec3(0, 0, 3));
}

//void Engine::renderLoop()
//{
//	//renderer.renderScene(scene, currentFrame);
//	renderer.update(0);
//}

void Engine::start()
{

	EC::get()->mainAllocator = &mainAllocator;
	EC::get()->window = &window;
	EC::get()->resourceManager = &resourceManager;
	EC::get()->scene = &scene;
	EC::get()->vulkanContext = &graphicsContext;

	try {
		graphicsContext.initialize();
	}
	catch (const std::exception& e) {
		Logger::logError(e.what());
		isEngineRunning = false;
	}

	coreSystems.registerSystem<ForwardRenderer>();
	coreSystems.initialize();

	initScene();

	isEngineRunning = true;

	while (isEngineRunning) {
		if (glfwWindowShouldClose(window.getNativeWindow())) {
			isEngineRunning = false;
		}
		window.display();
		coreSystems.update(0);
	}
	vkDeviceWaitIdle(graphicsContext.getDevice().logicalDevice);
}

Engine::~Engine()
{	
	renderer.shutdown();
	scene.release();
	resourceManager.release();
	graphicsContext.release();
	window.shutdown();
}

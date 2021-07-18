#include "Engine.h"

void Engine::start()
{
	window = new Window(800, 600, "Vulkan Engine");
	EngineContext::get()->window = window;

	isEngineRunning = true;

	try {
		vulkanContext.initialize();
		EngineContext::get()->vulkanContext = &vulkanContext;
	}
	catch (const std::exception& e) {
		Logger::logError(e.what());
		isEngineRunning = false;
	}

	while (isEngineRunning) {
		if (glfwWindowShouldClose(window->getNativeWindow())) {
			isEngineRunning = false;
		}
		window->display();
	}
}

Engine::~Engine()
{
	vulkanContext.release();
	window->shutdown();
}

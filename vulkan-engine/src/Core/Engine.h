#pragma once 

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include "Window.h"
#include "Logger.h"
#include "EngineContext.h"
#include "Renderer/VulkanContext.h"
#include "Renderer/Vertex.h"
#include "Scene/Scene.h"
#include "Renderer/ForwardRenderer.h"
#include "System.h"
#include "ResourceManagement/ResourceManager.h"

class Engine {
protected:
	Window window;
	vk::Context graphicsContext;
	StackAllocator mainAllocator;
	ResourceManager resourceManager;
	Scene scene;
	
	SystemManager coreSystems;


	bool isEngineRunning;

	ForwardRenderer renderer;

public:
	Engine();
	~Engine();

	void initScene();
	//void renderLoop();
	void start();
};

#endif // !ENGINE_H

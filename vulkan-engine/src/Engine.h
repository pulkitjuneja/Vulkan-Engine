#pragma once 

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include "Window.h"
#include "Logger.h"
#include "EngineContext.h"
#include "Renderer/VulkanContext.h"
#include "Renderer/BasicPipeline.h"
#include "Renderer/VulkanCommandBuffer.h"
#include "Renderer/Vertex.h"
#include "Scene/Scene.h"
#include "Renderer/SceneRenderer.h"
#include "ResourceManagement/ResourceManager.h"

class Engine {
protected:
	std::unique_ptr<Window> window;
	std::unique_ptr<VulkanContext> graphicsContext;
	bool isEngineRunning;
	size_t currentFrame = 0;

	BasicPipeline pipeline;

	Scene scene;
	SceneRenderer renderer;

	StackAllocator mainAllocator;
	ResourceManager* resourceManager;

public:
	Engine();
	~Engine();

	void initScene();
	void renderLoop();
	void start();
};

#endif // !ENGINE_H

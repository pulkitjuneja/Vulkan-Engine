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

class Engine {
protected:
	std::unique_ptr<Window> window;
	std::unique_ptr<VulkanContext> graphicsContext;
	bool isEngineRunning;
	size_t currentFrame = 0;

	std::vector<VulkanCommandBuffer> screenCommandBUffers;
	VulkanCommandBuffer mainCommandBuffer;
	BasicPipeline pipeline;

	std::vector<Mesh> Scene;

public:
	Engine() = default;
	~Engine();

	void initCommandBuffer();
	void initMeshes();
	void renderLoop();
	void start();
};

#endif // !ENGINE_H

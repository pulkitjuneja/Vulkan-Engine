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

class Engine {
protected:
	std::unique_ptr<Window> window;
	std::unique_ptr<VulkanContext> graphicsContext;
	bool isEngineRunning;
	size_t currentFrame = 0;

	std::vector<VulkanCommandBuffer> screenCommandBUffers;
	BasicPipeline pipeline;

public:
	Engine() = default;
	~Engine();

	void recordCommandBuffers();
	void renderLoop();
	void start();
};

#endif // !ENGINE_H

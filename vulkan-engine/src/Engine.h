#pragma once 

#ifndef ENGINE_H
#define ENGINE_H

#include "Window.h"
#include "Logger.h"
#include "VulkanContext.h"

class Engine {
protected:
	Window* window;
	bool isEngineRunning;
	VulkanContext vulkanContext;
public:
	Engine() = default;
	~Engine();
	void start();
};

#endif // !ENGINE_H

#pragma once

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <Memory>
#include "Allocator.h"

class Window;
class VulkanContext;
class ResourceManager;
class StackAllocator;

class EngineContext {
public:
	Window* window;
	VulkanContext* vulkanContext;
	StackAllocator* mainAllocator;
	ResourceManager* resourceManager;

	static EngineContext* get();
private:
	static EngineContext* instance;
	EngineContext() = default;
};

typedef EngineContext EC;

#endif
#pragma once

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <Memory>
#include "Allocator.h"

class Window;
class VulkanContext;

class EngineContext {
public:
	Window* window = nullptr;
	VulkanContext* vulkanContext;

	static EngineContext* get();
private:
	static EngineContext* instance;
	EngineContext() = default;
};

typedef EngineContext EC;

#endif
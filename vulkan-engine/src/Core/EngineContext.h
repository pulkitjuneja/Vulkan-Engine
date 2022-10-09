#pragma once

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <Memory>
#include "Allocator.h"

class Window;
class Context;
class ResourceManager;
class StackAllocator;
class Scene;

namespace vk {
	class Context;
}

class EngineContext {
public:
	Window* window;
	vk::Context* vulkanContext;
	StackAllocator* mainAllocator;
	ResourceManager* resourceManager;
	Scene* scene;

	static EngineContext* get();
private:
	static EngineContext* instance;
	EngineContext() = default;
};

typedef EngineContext EC;

#endif
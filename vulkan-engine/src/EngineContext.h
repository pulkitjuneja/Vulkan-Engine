#pragma once

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

class Window;
class VulkanContext;

class EngineContext {
public:
	Window* window = nullptr;
	VulkanContext* vulkanContext;

	static EngineContext* instance;
	static EngineContext* get();
private:
	EngineContext() = default;
};
#endif
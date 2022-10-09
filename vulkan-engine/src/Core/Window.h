#pragma once 


#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include "Constants.h"

struct  Window {
	GLFWwindow* window;
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

	void hookEvents();
	void shutdown();
	void display();
	GLFWwindow* getNativeWindow();
	Window();
};

#endif
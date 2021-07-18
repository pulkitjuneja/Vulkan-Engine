#pragma once 


#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class Window {
	GLFWwindow* window;
public:
	void hookEvents();
	void shutdown();
	void display();
	GLFWwindow* getNativeWindow();
	Window(int width, int height, std::string title);
};

#endif
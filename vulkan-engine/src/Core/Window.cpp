#include "Window.h"

Window::Window()
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to init GLFW\n");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, "Vulkan Engine", nullptr, nullptr);
}

void Window::shutdown()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::display()
{
    glfwPollEvents();
    //glfwSwapBuffers(window);
}

GLFWwindow* Window::getNativeWindow()
{
    return window;
}

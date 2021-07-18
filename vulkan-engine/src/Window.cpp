#include "Window.h"

Window::Window(int width, int height, std::string title)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to init GLFW\n");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void Window::shutdown()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::display()
{
    glfwPollEvents();
    glfwSwapBuffers(window);
}

GLFWwindow* Window::getNativeWindow()
{
    return window;
}

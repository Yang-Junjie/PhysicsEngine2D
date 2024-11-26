#include "Window.h"
#include <GLFW/glfw3.h>  // Include GLFW header here

Window::Window(int width, int height, const char* title) : width(width), height(height), title(title)
{
    initGLFW();
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    makeContextCurrent();
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(window);
}

void Window::update()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::makeContextCurrent() const
{
    glfwMakeContextCurrent(window);
}

GLFWwindow* Window::getWindow() const
{
    return window;
}

void Window::initGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
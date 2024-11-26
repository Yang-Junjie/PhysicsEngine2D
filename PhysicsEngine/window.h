#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

// Forward declaration of GLFWwindow
struct GLFWwindow;

class Window {
public:
    GLFWwindow* window;

    // Constructor initializes GLFW and creates a window
    Window(int width, int height, const char* title);

    // Destructor cleans up resources
    ~Window();

    // Function to check if the window should close
    bool shouldClose() const;

    // Function to swap buffers and poll events
    void update();

    // Function to set the context as current
    void makeContextCurrent() const;
    GLFWwindow* getWindow() const;

private:
    int width;
    int height;
    const char* title;

    // Private function to initialize GLFW
    void initGLFW();
};

#endif // WINDOW_H
#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

class ImGuiManager {
public:
    ImGuiManager(GLFWwindow* window);
    ~ImGuiManager();

    void newFrame();
    void render();
    void handleInput();

private:
    GLFWwindow* window;
};

#endif // IMGUI_MANAGER_H
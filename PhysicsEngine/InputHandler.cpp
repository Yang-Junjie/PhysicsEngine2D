#include "InputHandler.h"


// 定义全局变量
bool rightMousePressed = false;
bool leftMousePressed = false;
float lastX = 0.0f, lastY = 0.0f;
glm::vec2 translation = glm::vec2(0.0f, 0.0f);
float OE_ZOOM = 0.2f; // 缩放因子


// 实现鼠标按钮回调函数
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rightMousePressed = true;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        rightMousePressed = false;
    }
}

// 实现鼠标位置回调函数
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (rightMousePressed) {
        float x = static_cast<float>(xpos);
        float y = static_cast<float>(ypos);
        float dx = x - lastX;
        float dy = lastY - y; // 注意 y 轴方向相反
        translation.x += dx * 0.0025f;
        translation.y += dy * 0.0025f;
        lastX = x;
        lastY = y;
    }
}

// 实现视口大小改变的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 实现鼠标滚轮回调函数
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    OE_ZOOM += static_cast<float>(yoffset * 0.03f*OE_ZOOM); // 调整缩放因子
    if (OE_ZOOM < 0.03f) {
        OE_ZOOM = 0.03f; // 防止缩放因子过小
    }
}
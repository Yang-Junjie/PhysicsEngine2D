#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


// 声明全局变量
extern bool rightMousePressed;
extern float lastX, lastY;
extern glm::vec2 translation;
extern float OE_ZOOM;


// 鼠标按钮回调函数
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// 鼠标位置回调函数
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

// 视口大小改变的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// 鼠标滚轮回调函数
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

#endif // CALLBACKS_H
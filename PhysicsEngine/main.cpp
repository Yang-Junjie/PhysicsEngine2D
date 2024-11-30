#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <chrono>
#include "Window.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "ImGuiManager.h"
#include "InputHandler.h"
#include "math.h"

#include "body.h"
#include "type.h"
#include "world.h"
#include "collision.h"

oeWorld world;
Renderer renderer;
bool lock120FPS = true;
bool intersect = false;

oeBody* selectedBody = nullptr;

void dome1() {
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 1; j++) {
            BoxType data;
            data.position += oeVec2(0.15f * i, 0.15f * j);
            world.CreatBox(data);
        }
    }
}

void dome2() {
    BoxType data;
    world.CreatBox(data);
    world.FindBody(0).Rotation(oeVec2::AngleToRadian(45));
}

void dome3() {
    int count = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 1; j++) {
            BoxType data;
            data.position += oeVec2(0.5f * i, 0.5f * j);
            world.CreatBox(data);
            world.FindBody(count).Rotation(oeVec2::AngleToRadian(45));
            count++;
        }
    }
}

void ContorlAllBody() {
    for (auto& body : *(world.GetBodysList())) {
        body.Rotation(oeVec2::AngleToRadian(1));
    }
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
int main() {
    try {
        // Create a window
        Window window(1000, 800, "PhysicsEngine");

        // Load OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // 设置回调函数
        glfwSetFramebufferSizeCallback(window.getWindow(), framebuffer_size_callback);
        glfwSetMouseButtonCallback(window.getWindow(), mouseButtonCallback);
        glfwSetCursorPosCallback(window.getWindow(), cursorPositionCallback);
        glfwSetScrollCallback(window.getWindow(), scrollCallback); // 注册鼠标滚轮回调函数
        glfwSetKeyCallback(window.getWindow(), keyCallback);

        // Initialize ImGui
        ImGuiManager imguiManager(window.getWindow());

        // Compile and link shaders
        Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

        dome3();

        auto lastFrameTime = std::chrono::high_resolution_clock::now(); // 上一帧的时间
        float startTime = static_cast<float>(glfwGetTime()); // 记录程序启动时间

        // 初始化选中的物体
        if (world.GetBodyNum()>0) {
            selectedBody = &world.FindBody(0);
        }

        while (!window.shouldClose()) {
            // 计算当前帧的时间
            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
            lastFrameTime = currentFrameTime;

            // Input handling
            imguiManager.handleInput();

            // Start the Dear ImGui frame
            imguiManager.newFrame();

            // 获取窗口的当前尺寸
            int width, height;
            glfwGetFramebufferSize(window.getWindow(), &width, &height);

            // Rendering commands here
            renderer.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            renderer.clear();

            shader.use();
            shader.scale(OE_ZOOM);

            // 设置投影矩阵
            shader.setupProjection(width, height);

            // 设置视图矩阵
            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, 0.0f));
            shader.setMat4("view", view);

            // 绘制坐标轴
            float color[4] = { 1.0f, 0.0f, 0.0f, 0.3f };
            renderer.drawLine(oeVec2(0, -100), oeVec2(0, 100), color);
            renderer.drawLine(oeVec2(-100, 0), oeVec2(100, 0), color);

           ContorlAllBody();

            world.RenderBody(renderer);

            // ImGui rendering
            {
                ImGui::Begin("console");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Text("num of Body : %d", world.GetBodyNum());

                static int bodyid = 0; // 创建一个变量来保存输入的整型值
                static bool button_clicked = false;
                static bool checkbox_state = false;
                // 勾选框
                ImGui::Checkbox("drawAABB", &checkbox_state);
                if (checkbox_state) { 
                    world.RenderAABB(renderer);
                    
                }
                
                ImGui::Checkbox("Lock 120FPS", &lock120FPS);
                ImGui::InputInt("Find body value", &bodyid);
                if (bodyid + 1 <= world.GetBodyNum() && bodyid>=0) {
                    ImGui::Text("Body %d, position:(%.3f, %.3f)", bodyid, world.FindBody(bodyid).mass_center_.x, world.FindBody(bodyid).mass_center_.y);
                    ImGui::Text("Body %d, aabbmax:(%.3f, %.3f)", bodyid, world.FindBody(bodyid).aabb_.max.x, world.FindBody(bodyid).aabb_.max.y);
                    ImGui::Text("Body %d, aabbmin:(%.3f, %.3f)", bodyid, world.FindBody(bodyid).aabb_.min.x, world.FindBody(bodyid).aabb_.min.y);


                }
                else {
                    ImGui::Text("Body %d is nonexistence", bodyid);
                }
                static int prevBodyid = -1; // 用于记录上一次的 bodyid
                if (bodyid != prevBodyid) {
                    prevBodyid = bodyid;
                    if (bodyid >= 0 && bodyid < world.GetBodyNum()) {
                        selectedBody = &world.FindBody(bodyid);
                    }
                    else {
                        selectedBody = nullptr;
                    }
                }
               
                ImGui::End();
            }


            imguiManager.render();

            // 如果启用了120FPS锁定，则确保每帧至少消耗1/120秒的时间
            if (lock120FPS) {
                const float targetFPS = 120.0f;
                const float targetFrameTime = 1.0f / targetFPS;

                // 计算剩余需要等待的时间（以秒为单位）
                float remainingTime = targetFrameTime - deltaTime;
                if (remainingTime > 0.0f) {
                    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(remainingTime * 1000000)));
                }
            }
            
            world.Interation(deltaTime);
            // Swap buffers and poll events
            window.update();
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

// 键盘回调函数
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (selectedBody != nullptr) {
            float moveSpeed = 0.05f; // 移动速度
            switch (key) {
            case GLFW_KEY_UP:
                selectedBody->Move(oeVec2(0, moveSpeed));
                break;
            case GLFW_KEY_DOWN:
                selectedBody->Move(oeVec2(0, -moveSpeed));
                break;
            case GLFW_KEY_LEFT:
                selectedBody->Move(oeVec2(-moveSpeed, 0));
                break;
            case GLFW_KEY_RIGHT:
                selectedBody->Move(oeVec2(moveSpeed, 0));
                break;
            }
        }
    }
}
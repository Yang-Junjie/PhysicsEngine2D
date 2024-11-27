#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "ImGuiManager.h"
#include "math.h"
#include "body.h"
#include "type.h"
#include "world.h"
#include "InputHandler.h"


float sca = 1.0f; 
int main()
{
    try {
        // Create a window
        Window window(1000, 800, "PhysicsEngine");

        // Load OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // 设置回调函数
        glfwSetFramebufferSizeCallback(window.getWindow(), framebuffer_size_callback);
        glfwSetMouseButtonCallback(window.getWindow(), mouseButtonCallback);
        glfwSetCursorPosCallback(window.getWindow(), cursorPositionCallback);
        glfwSetScrollCallback(window.getWindow(), scrollCallback); // 注册鼠标滚轮回调函数

        // Initialize ImGui
        ImGuiManager imguiManager(window.getWindow());

        // Compile and link shaders
        Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
        Renderer renderer;

        // My world
        oeWorld world;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                BoxType data;
                data.position += oeVec2(0.15 * i, 0.15 * j);
                world.CreatBox(data);
            }
        }

        float lastFrameTime = 0.0f; // 上一帧的时间
        float startTime = glfwGetTime(); // 记录程序启动时间

        while (!window.shouldClose())
        {
            // 计算当前帧的时间
            float currentFrameTime = glfwGetTime();
            float deltaTime = currentFrameTime - lastFrameTime;
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


            //绘制坐标轴
            float color[4] = { 1.0f, 0.0f, 0.0f ,0.3f };
            renderer.drawLine(oeVec2(0,-100), oeVec2(0,100), color);
            renderer.drawLine(oeVec2(-100,0), oeVec2(100, 0), color);
            world.RenderBody(renderer);

            //// 基于时间步长更新位置
            int count = 0;
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    float speed = 0.1f; // 每秒移动的速度
                    world.FindBody(count).mass_center_.x += speed * deltaTime;
                    count++;
                }
            }

            // ImGui rendering
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            imguiManager.render();

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
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
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

oeWorld world;
Renderer renderer;
bool lock120FPS = true;

void dome1() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            BoxType data;
            data.position += oeVec2(0.15 * i, 0.15 * j);
            world.CreatBox(data);
        }
    }
}


void dome2() { 
    BoxType data;
   // data.width = 0.5;
    world.CreatBox(data);
   // world.FindBody(0).Move(oeVec2(0.3f, 0.0f));
    world.FindBody(0).Rotation(oeVec2::AngleToRadian(45));


}

void dome3() {
    int count = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 17; j++) {
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

void drawaabb() {
    for (auto& body : *(world.GetBodysList())) {
        //body.Rotation(oeVec2::AngleToRadian(0.1));
        body.GetAABB();
        //std::cout << body.aabb_.lowerBound;
        renderer.drawAABB(body.aabb_.upperBound,body.aabb_.lowerBound, body.color_);
    }
}

 
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
        


        dome3();
        
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




            //ContorlAllBody();
           

            world.RenderBody(renderer);

          
            
           
            
           //  ImGui rendering
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
                    drawaabb();
                }
                
                ImGui::Checkbox("Lock 120FPS", &lock120FPS);
                ImGui::InputInt("Find body value", &bodyid);
                if (bodyid + 1 <=world.GetBodyNum()) {
                    ImGui::Text("Body %d,position:(%.3f,%.3f)", bodyid, world.FindBody(bodyid).mass_center_.x, world.FindBody(bodyid).mass_center_.y);
                }
                else {
                    ImGui::Text("Body %d is nonexistence", bodyid);
                }
                ImGui::End();
            }

            imguiManager.render();

            // 如果启用了120FPS锁定，则确保每帧至少消耗1/120秒的时间
            if (lock120FPS)
            {
                const float targetFPS = 120.0f;
                const float targetFrameTime = 1.0f / targetFPS;

                // 计算剩余需要等待的时间（以秒为单位）
                float remainingTime = targetFrameTime - deltaTime;
                if (remainingTime > 0.0f)
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(remainingTime * 1000000)));
                }
            }
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
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "ImGuiManager.h"
#include "math.h"
#include "body.h"
#include "type.h"
#include "world.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main()
{
    try {
        // Create a window
        Window window(800, 800, "PhysicsEngine");

        // Load OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

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
                data.position  += oeVec2(0.15*i,0.15*j);
                world.CreatBox(data);
            }
        }
      

       
       
        float lastFrameTime = 0.0f; // 上一帧的时间
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

            // Rendering commands here
            renderer.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            renderer.clear();

            
            shader.use();
            shader.scale(0.5);




            world.RenderBody(renderer);



            // 基于时间步长更新位置
            //float speed = 5.0f; // 每秒移动的速度
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

        // Cleanup
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
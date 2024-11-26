#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Shader {
public:
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // Use the shader program
    void use() const;

    void scale(float coeff);

private:
    // Utility function to check shader compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type);

    // Utility function to read shader files
    std::string readFile(const std::string& filePath);
};

#endif // SHADER_H
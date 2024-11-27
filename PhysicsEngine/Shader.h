#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;
    void scale(float coeff);
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setupProjection(int width, int height) const;

private:
    void checkCompileErrors(unsigned int shader, const std::string& type);
    std::string readFile(const std::string& filePath);
};
#endif // SHADER_H
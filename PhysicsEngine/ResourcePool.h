#include <vector>
#include<glad/glad.h>
#include <memory>

class OpenGLObjectPool {
public:
    OpenGLObjectPool() {
        // 预先创建一定数量的 VAO 和 VBO
        for (int i = 0; i < 10; ++i) {
            unsigned int vao;
            glGenVertexArrays(1, &vao);
            vaoPool.push_back(vao);

            unsigned int vbo;
            glGenBuffers(1, &vbo);
            vboPool.push_back(vbo);
        }
    }

    ~OpenGLObjectPool() {
        // 删除所有 VAO 和 VBO
        for (auto vao : vaoPool) {
            glDeleteVertexArrays(1, &vao);
        }
        for (auto vbo : vboPool) {
            glDeleteBuffers(1, &vbo);
        }
    }

    unsigned int getVAO() {
        if (vaoPool.empty()) return 0;
        unsigned int vao = vaoPool.back();
        vaoPool.pop_back();
        return vao;
    }

    void returnVAO(unsigned int vao) {
        vaoPool.push_back(vao);
    }

    unsigned int getVBO() {
        if (vboPool.empty()) return 0;
        unsigned int vbo = vboPool.back();
        vboPool.pop_back();
        return vbo;
    }

    void returnVBO(unsigned int vbo) {
        vboPool.push_back(vbo);
    }

private:
    std::vector<unsigned int> vaoPool;
    std::vector<unsigned int> vboPool;
};
#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <glad/glad.h>
#include <vector>

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void addBuffer(const float* vertices, size_t count, int numComponents);

private:
    unsigned int vaoID;
    unsigned int vboID;
};

#endif // VERTEXARRAY_H
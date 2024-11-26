#include "VertexArray.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &vboID);
}

void VertexArray::bind() const {
    glBindVertexArray(vaoID);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::addBuffer(const float* vertices, size_t count, int numComponents) {
    bind();

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, numComponents, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    unbind();
}
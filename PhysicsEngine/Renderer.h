#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include "ResourcePool.h"
#include "body.h"
#include "math.h"

class Renderer {

   
public:

    void setClearColor(float r, float g, float b, float a);
    void clear();

    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void drawRectangle(float x, float y, float width, float height, const float* color);

    void drawCircle(float centerX, float centerY, float radius, const float* color, int segments = 36);
    void drawCircle(oeVec2 center, float radius, const float* color, int segments = 36);

    void drawLine(const float startX, const float startY, const float endX, const float endY, const float* color);
    void drawLine(const oeVec2 start, const oeVec2 end, const float* color);
    void drawVector(const oeVec2 start, const oeVec2 end, const float* color);

    void drawPolygon(const oeVec2* vertices, int vertices_count, const float* color);
    void drawHollowPolygon(const oeVec2* vertices, int vertices_count, const float* color);

    void drawAABB(const oeAABB aabb, const float color[4]);

    //OpenGLObjectPool objectPool;

};

#endif // RENDERER_H


#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

uniform float scaling;
out vec4 ourColor;
uniform mat4 projection;
uniform mat4 view;
void main()
{
    gl_Position =projection* view *vec4(aPos.x*scaling,aPos.y*scaling,aPos.z*scaling, 1.0);
    ourColor = aColor;
}

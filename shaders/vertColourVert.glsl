#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inCol;

out vec3 vertCol;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(inPosition.xyz, 1.0);
    vertCol = inCol;
}

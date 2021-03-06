#version 400 core

layout (location = 0) in vec3 inPosition;

uniform mat4 MVP;

void main()
{
    gl_Position  = MVP * vec4(inPosition, 1.0);
}

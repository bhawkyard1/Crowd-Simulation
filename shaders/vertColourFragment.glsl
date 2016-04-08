#version 410 core

in vec3 vertCol;

layout(location = 0) out vec4 fragColour;

void main()
{
    fragColour = vec4(vertCol.rgb,1.0);
}

#version 400 core

in vec4 fragCoord;

out vec4 fragColour;

uniform vec4 inColour;

void main()
{
    fragColour = inColour;
}

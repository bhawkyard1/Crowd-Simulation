#version 410 core

in vec2 UV;
in vec3 normal;

layout(location = 0) out vec4 fragColour;

uniform vec4 inColour;
//uniform vec3 ambientLightPos;

void main()
{
    vec3 ambientLightPos = vec3(0.0, 2.0, 2.0);

    vec3 N = normalize(normal);

    fragColour = inColour;
    fragColour.rgb *= dot(N, ambientLightPos);
    fragColour.a = 1.0;
}

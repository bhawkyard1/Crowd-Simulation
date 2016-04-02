#version 410 core

in vec2 UV;
in vec3 normal;

layout(location = 0) out vec4 fragColour;

void main()
{
    vec3 ambientLightPos = vec3(0.0, 2.0, 2.0);

    vec3 N = normalize(normal);

    fragColour = vec4(0.06);
    fragColour.rgb *= dot(N, ambientLightPos);
    fragColour.a = 1.0;
}

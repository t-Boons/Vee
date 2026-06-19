#version 450

layout(location = 0) in vec3 worldPos;

layout(location = 0) out vec4 outColor;


layout(set = 0, binding = 1) uniform samplerCube tex;

void main()
{
    vec3 dir = normalize(worldPos);
    vec3 color = texture(tex, dir).xyz;
    outColor = vec4(color, 1.0);
}
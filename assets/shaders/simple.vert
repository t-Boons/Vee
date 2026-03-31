#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 vp;
    mat4 m;
} ubo;

void main()
{
    vec4 pos = vec4(inPosition, 1.0);
    gl_Position = ubo.vp * ubo.m * pos;

    fragColor = vec3(1.0f, 1.0f, 0.0f);
}
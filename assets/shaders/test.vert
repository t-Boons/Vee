#version 450

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec2 outTexCoords;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 model;
    mat3 normalMatrix;
} ubo;

void main()
{
    mat4 mvp = ubo.projection * ubo.view * ubo.model;

    vec4 worldPos = ubo.model * vec4(vertices, 1.0);

    outTexCoords = texCoords;
    gl_Position = mvp * vec4(vertices, 1.0);
}
#version 450

layout(location = 0) in vec3 vertices;

layout(location = 0) out vec3 worldPos;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 model;
    mat3 normalMatrix;
} ubo;

void main()
{
worldPos = vec3(vertices.x, -vertices.y, vertices.z);

    vec4 pos = ubo.projection * mat4(mat3(ubo.view)) * vec4(vertices, 1.0);
    gl_Position = pos.xyww;
}
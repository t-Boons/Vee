#version 450

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec4 tangents;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoords;
layout(location = 2) out vec4 outTangent;
layout(location = 3) out vec3 outBitangent;
layout(location = 4) out vec3 outFragPos;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 model;
    mat4 normalMatrix;
    vec4 camPos;
} ubo;

void main()
{
    mat4 mvp = ubo.projection * ubo.view * ubo.model;

    vec4 worldPos = ubo.model * vec4(vertices, 1.0);

    outFragPos = worldPos.xyz;
    outTexCoords = texCoords;

    outNormal = normalize(mat3(ubo.normalMatrix) * normals);

    vec3 T = normalize(mat3(ubo.model) * tangents.xyz);
    outTangent = vec4(T, tangents.w);

    vec3 bitangent = cross(normals, tangents.xyz) * tangents.w;
    outBitangent = normalize(mat3(ubo.normalMatrix) * bitangent);

    gl_Position = mvp * vec4(vertices, 1.0);
}
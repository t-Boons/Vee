#version 450

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;


layout(set = 0, binding = 1) uniform sampler2D tex;

void main()
{
    vec3 color = texture(tex, inTexCoords).xyz;
    outColor = vec4(color, 1.0);
}
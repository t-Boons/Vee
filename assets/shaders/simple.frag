#version 450

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec3 inFragPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D albedoMap;
layout(set = 0, binding = 2) uniform sampler2D normalMap;
layout(set = 0, binding = 3) uniform sampler2D roughnessMap;

float dot01(vec3 a, vec3 b)
{
    return max(dot(a, b), 0.0);
}

vec3 F_Schlick(vec3 F0, float VdotH)
{
    return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
}

vec3 BRDF_Lambertian(vec3 albedo)
{
    return albedo / 3.14159265;
}

vec3 BRDF_CookTorrence(float NdotL, float NdotV, vec3 F, float roughness)
{
    return F;
}

vec3 MapNormal(vec3 texNormal, vec3 T, vec3 B, vec3 N, float scale)
{
    vec3 n = texNormal * 2.0 - 1.0;
    n.xy *= scale;
    mat3 TBN = mat3(normalize(T), normalize(B), normalize(N));
    return normalize(TBN * n);
}

void main()
{
    vec3 normal = MapNormal(texture(normalMap

    vec3 texColor = texture(albedoMap, inTexCoords).xyz;
    outColor = vec4(texColor, 1.0f);
}
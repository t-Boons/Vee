#version 450

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec3 inFragPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D albedoMap;
layout(set = 0, binding = 2) uniform sampler2D normalMap;
layout(set = 0, binding = 3) uniform sampler2D metallicRoughnessMap;
layout(set = 0, binding = 4) uniform sampler2D emissionMap;
layout(set = 0, binding = 5) uniform samplerCube skyboxMap;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 model;
    mat4 normalMatrix;
    vec4 cameraPos;
} ubo;

const vec3  LIGHT_DIR       = normalize(vec3(1.0, -2.0, 1.0));
const vec3  LIGHT_COLOR     = vec3(1.0, 0.9, 0.7);
const float LIGHT_INTENSITY = 1.0;

const float PI = 3.14159265359;

float dot01(vec3 a, vec3 b)
{
    return max(dot(a, b), 0.0);
}

vec3 F_Schlick(vec3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float D_GGX(float NdotH, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float d  = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (PI * d * d);
}

float G_SchlickGGX(float NdotX, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotX / (NdotX * (1.0 - k) + k);
}

float G_Smith(float NdotV, float NdotL, float roughness)
{
    return G_SchlickGGX(NdotV, roughness) * G_SchlickGGX(NdotL, roughness);
}

vec3 BRDF_Lambertian(vec3 albedo)
{
    return albedo / PI;
}

vec3 BRDF_CookTorrance(float NdotL, float NdotV, float NdotH, float VdotH,
                        vec3 F0, float roughness)
{
    float D = D_GGX(NdotH, roughness);
    float G = G_Smith(NdotV, NdotL, roughness);
    vec3  F = F_Schlick(F0, VdotH);
    return (D * G * F) / (4.0 * NdotV * NdotL + 0.0001);
}

vec3 MapNormal(vec3 texNormal, vec3 T, vec3 B, vec3 N, float scale)
{
    vec3 n = texNormal * 2.0 - 1.0;
    n.xy  *= scale;
    mat3 TBN = mat3(normalize(T), normalize(B), normalize(N));
    return normalize(TBN * n);
}

void main()
{
    vec3 albedo = pow(texture(albedoMap, inTexCoords).rgb, vec3(2.2));
    vec2  mr        = texture(metallicRoughnessMap, inTexCoords).rg;
    float metallic  = mr.r;
    float roughness = clamp(mr.g, 0.01, 1.0);
    vec3 emission = pow(texture(emissionMap, inTexCoords).rgb, vec3(2.2));

    vec3 N = MapNormal(texture(normalMap, inTexCoords).rgb, inTangent.xyz, inBitangent, inNormal, 1.0);

    vec3 V = normalize(ubo.cameraPos.xyz - inFragPos);
    vec3 L = LIGHT_DIR;
    vec3 H = normalize(V + L);

    float NdotL = dot01(N, L);
    float NdotV = dot01(N, V);
    float NdotH = dot01(N, H);
    float VdotH = dot01(V, H);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = F_Schlick(F0, VdotH);
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    vec3 specular = BRDF_CookTorrance(NdotL, NdotV, NdotH, VdotH, F0, roughness);

    vec3 R = reflect(-V, N);
    vec3 envColor = texture(skyboxMap, R).rgb;
    vec3 envReflection = envColor * F * (1.0 - roughness) * (1.0f - metallic);

    vec3 ambient = albedo * envReflection * 0.03;
    vec3 radiance = LIGHT_COLOR * LIGHT_INTENSITY;

    vec3 gi = envColor * kD * BRDF_Lambertian(albedo);

    vec3 color = specular * radiance + ambient + emission + gi;

    outColor = vec4(color, 1.0);
}

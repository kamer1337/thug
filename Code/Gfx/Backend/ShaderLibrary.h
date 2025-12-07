// Shader Library - Predefined Modern Shaders for THUG Graphics Engine
// Provides a collection of modern shaders for various rendering techniques

#ifndef __GFX_BACKEND_SHADER_LIBRARY_H
#define __GFX_BACKEND_SHADER_LIBRARY_H

namespace Gfx
{
namespace Backend
{

// Shader source code strings for modern rendering

// ============================================================================
// Basic Shaders
// ============================================================================

// Basic Vertex Shader - Simple transformation with color
static const char* SHADER_BASIC_VERTEX = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    vPosition = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal;
    vTexCoord = aTexCoord;
    vColor = aColor;
    
    gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
}
)";

// Basic Fragment Shader - Simple color with texture
static const char* SHADER_BASIC_FRAGMENT = R"(
#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

void main()
{
    vec4 baseColor = uUseTexture ? texture(uTexture, vTexCoord) : uColor;
    baseColor *= vColor;
    FragColor = baseColor;
}
)";

// ============================================================================
// Phong Lighting Shaders
// ============================================================================

// Phong Vertex Shader
static const char* SHADER_PHONG_VERTEX = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;

void main()
{
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    vPosition = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vTexCoord = aTexCoord;
    
    gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
}
)";

// Phong Fragment Shader - Classic Phong lighting
static const char* SHADER_PHONG_FRAGMENT = R"(
#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

// Material properties
uniform sampler2D uAlbedoMap;
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;
uniform bool uUseTexture;

// Lighting
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform float uLightIntensity;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vPosition);
    vec3 viewDir = normalize(uViewPos - vPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // Ambient
    vec3 ambient = uAmbient * uLightColor;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uDiffuse * uLightColor;
    
    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = spec * uSpecular * uLightColor;
    
    // Combine
    vec3 result = (ambient + diffuse + specular) * uLightIntensity;
    
    if (uUseTexture)
    {
        vec4 texColor = texture(uAlbedoMap, vTexCoord);
        result *= texColor.rgb;
        FragColor = vec4(result, texColor.a);
    }
    else
    {
        FragColor = vec4(result, 1.0);
    }
}
)";

// ============================================================================
// PBR (Physically Based Rendering) Shaders
// ============================================================================

// PBR Vertex Shader
static const char* SHADER_PBR_VERTEX = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out mat3 vTBN;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;

void main()
{
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    vPosition = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vTexCoord = aTexCoord;
    
    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(uNormalMatrix * aTangent);
    vec3 N = vNormal;
    vec3 B = cross(N, T);
    vTBN = mat3(T, B, N);
    
    gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
}
)";

// PBR Fragment Shader - Physically Based Rendering
static const char* SHADER_PBR_FRAGMENT = R"(
#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in mat3 vTBN;

out vec4 FragColor;

const float PI = 3.14159265359;

// PBR Material
uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAOMap;

uniform vec3 uAlbedo;
uniform float uMetallic;
uniform float uRoughness;
uniform float uAO;
uniform bool uUseTextures;

// Lighting
uniform vec3 uLightPositions[4];
uniform vec3 uLightColors[4];
uniform vec3 uViewPos;
uniform int uNumLights;

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

// Geometry Function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel-Schlick Approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // Sample material properties
    vec3 albedo = uUseTextures ? texture(uAlbedoMap, vTexCoord).rgb : uAlbedo;
    float metallic = uUseTextures ? texture(uMetallicMap, vTexCoord).r : uMetallic;
    float roughness = uUseTextures ? texture(uRoughnessMap, vTexCoord).r : uRoughness;
    float ao = uUseTextures ? texture(uAOMap, vTexCoord).r : uAO;
    
    vec3 N = normalize(vNormal);
    if (uUseTextures)
    {
        N = texture(uNormalMap, vTexCoord).rgb;
        N = N * 2.0 - 1.0;
        N = normalize(vTBN * N);
    }
    
    vec3 V = normalize(uViewPos - vPosition);
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < uNumLights && i < 4; ++i)
    {
        vec3 L = normalize(uLightPositions[i] - vPosition);
        vec3 H = normalize(V + L);
        float distance = length(uLightPositions[i] - vPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = uLightColors[i] * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    // Ambient lighting
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    
    // HDR tonemapping (Reinhard)
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
)";

// ============================================================================
// Shadow Mapping Shaders
// ============================================================================

// Shadow Map Vertex Shader
static const char* SHADER_SHADOW_VERTEX = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModelMatrix;

void main()
{
    gl_Position = uLightSpaceMatrix * uModelMatrix * vec4(aPosition, 1.0);
}
)";

// Shadow Map Fragment Shader
static const char* SHADER_SHADOW_FRAGMENT = R"(
#version 330 core

void main()
{
    // Depth is automatically written to depth buffer
}
)";

// Shadow Mapping Vertex Shader (with shadows)
static const char* SHADER_SHADOW_MAPPING_VERTEX = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vPositionLightSpace;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uLightSpaceMatrix;
uniform mat3 uNormalMatrix;

void main()
{
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    vPosition = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vTexCoord = aTexCoord;
    vPositionLightSpace = uLightSpaceMatrix * worldPos;
    
    gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
}
)";

// Shadow Mapping Fragment Shader (with shadows)
static const char* SHADER_SHADOW_MAPPING_FRAGMENT = R"(
#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vPositionLightSpace;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uShadowMap;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform bool uUseTexture;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    // Bias to prevent shadow acne
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // PCF (Percentage Closer Filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vPosition);
    vec3 viewDir = normalize(uViewPos - vPosition);
    
    // Ambient
    vec3 ambient = 0.15 * uLightColor;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * uLightColor;
    
    // Shadow
    float shadow = ShadowCalculation(vPositionLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    
    vec4 color = uUseTexture ? texture(uTexture, vTexCoord) : vec4(1.0);
    FragColor = vec4(lighting * color.rgb, color.a);
}
)";

// ============================================================================
// Post-Processing Shaders
// ============================================================================

// Screen Quad Vertex Shader (for post-processing)
static const char* SHADER_SCREEN_QUAD_VERTEX = R"(
#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
)";

// Bloom Fragment Shader
static const char* SHADER_BLOOM_FRAGMENT = R"(
#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uScene;
uniform sampler2D uBloomBlur;
uniform float uBloomStrength;
uniform float uExposure;

void main()
{
    vec3 hdrColor = texture(uScene, vTexCoord).rgb;
    vec3 bloomColor = texture(uBloomBlur, vTexCoord).rgb;
    
    hdrColor += bloomColor * uBloomStrength;
    
    // Tone mapping (Reinhard)
    vec3 mapped = vec3(1.0) - exp(-hdrColor * uExposure);
    
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));
    
    FragColor = vec4(mapped, 1.0);
}
)";

// Gaussian Blur Fragment Shader
static const char* SHADER_BLUR_FRAGMENT = R"(
#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uImage;
uniform bool uHorizontal;
uniform float uWeights[5];

void main()
{
    vec2 texOffset = 1.0 / textureSize(uImage, 0);
    vec3 result = texture(uImage, vTexCoord).rgb * uWeights[0];
    
    if (uHorizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(uImage, vTexCoord + vec2(texOffset.x * i, 0.0)).rgb * uWeights[i];
            result += texture(uImage, vTexCoord - vec2(texOffset.x * i, 0.0)).rgb * uWeights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(uImage, vTexCoord + vec2(0.0, texOffset.y * i)).rgb * uWeights[i];
            result += texture(uImage, vTexCoord - vec2(0.0, texOffset.y * i)).rgb * uWeights[i];
        }
    }
    
    FragColor = vec4(result, 1.0);
}
)";

// SSAO (Screen Space Ambient Occlusion) Fragment Shader
static const char* SHADER_SSAO_FRAGMENT = R"(
#version 330 core

in vec2 vTexCoord;
out float FragColor;

uniform sampler2D uPositionMap;
uniform sampler2D uNormalMap;
uniform sampler2D uNoiseMap;

uniform vec3 uSamples[64];
uniform mat4 uProjectionMatrix;
uniform vec2 uNoiseScale;
uniform float uRadius;
uniform float uBias;

void main()
{
    vec3 fragPos = texture(uPositionMap, vTexCoord).xyz;
    vec3 normal = normalize(texture(uNormalMap, vTexCoord).rgb);
    vec3 randomVec = normalize(texture(uNoiseMap, vTexCoord * uNoiseScale).xyz);
    
    // Create TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    for (int i = 0; i < 64; ++i)
    {
        vec3 samplePos = TBN * uSamples[i];
        samplePos = fragPos + samplePos * uRadius;
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = uProjectionMatrix * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = texture(uPositionMap, offset.xy).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + uBias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = occlusion;
}
)";

} // namespace Backend
} // namespace Gfx

#endif // __GFX_BACKEND_SHADER_LIBRARY_H

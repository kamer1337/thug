// Modern Renderer - Implements modern rendering techniques
// Provides deferred rendering, post-processing, and advanced effects

#ifndef __GFX_BACKEND_MODERN_RENDERER_H
#define __GFX_BACKEND_MODERN_RENDERER_H

#include "GraphicsBackend.h"
#include "ShaderManager.h"
#include <vector>

namespace Gfx
{
namespace Backend
{

// Framebuffer attachment types
enum AttachmentType
{
    ATTACHMENT_COLOR,
    ATTACHMENT_DEPTH,
    ATTACHMENT_DEPTH_STENCIL
};

// Framebuffer structure
struct Framebuffer
{
    unsigned int fbo;
    std::vector<Texture*> colorAttachments;
    Texture* depthAttachment;
    int width;
    int height;
    
    Framebuffer()
        : fbo(0)
        , depthAttachment(nullptr)
        , width(0)
        , height(0)
    {}
};

// G-Buffer for deferred rendering
struct GBuffer
{
    Framebuffer* framebuffer;
    Texture* positionTexture;      // World space position
    Texture* normalTexture;        // World space normal
    Texture* albedoSpecTexture;    // Albedo (RGB) + Specular (A)
    Texture* depthTexture;         // Depth
    
    GBuffer()
        : framebuffer(nullptr)
        , positionTexture(nullptr)
        , normalTexture(nullptr)
        , albedoSpecTexture(nullptr)
        , depthTexture(nullptr)
    {}
};

// Light data for deferred rendering
struct Light
{
    float position[3];
    float color[3];
    float intensity;
    float radius;
    int type; // 0=point, 1=directional, 2=spot
    
    Light()
        : intensity(1.0f)
        , radius(10.0f)
        , type(0)
    {
        position[0] = position[1] = position[2] = 0.0f;
        color[0] = color[1] = color[2] = 1.0f;
    }
};

// Post-processing effects configuration
struct PostProcessConfig
{
    bool enableBloom;
    bool enableSSAO;
    bool enableHDR;
    bool enableFXAA;
    
    float bloomThreshold;
    float bloomStrength;
    float exposure;
    float ssaoRadius;
    float ssaoBias;
    
    PostProcessConfig()
        : enableBloom(false)
        , enableSSAO(false)
        , enableHDR(true)
        , enableFXAA(false)
        , bloomThreshold(1.0f)
        , bloomStrength(0.04f)
        , exposure(1.0f)
        , ssaoRadius(0.5f)
        , ssaoBias(0.025f)
    {}
};

// Modern Renderer class
class ModernRenderer
{
public:
    ModernRenderer(IGraphicsBackend* backend, ShaderManager* shaderManager);
    ~ModernRenderer();
    
    // Initialize renderer
    bool Initialize(int width, int height);
    
    // Shutdown and cleanup
    void Shutdown();
    
    // Resize buffers
    void Resize(int width, int height);
    
    // Deferred rendering
    void BeginGeometryPass();
    void EndGeometryPass();
    void BeginLightingPass();
    void EndLightingPass();
    
    // Add lights for deferred rendering
    void AddLight(const Light& light);
    void ClearLights();
    
    // Post-processing
    void ApplyPostProcessing(Texture* inputTexture, Framebuffer* outputFBO);
    
    // Individual post-process effects
    Texture* ApplyBloom(Texture* inputTexture);
    Texture* ApplySSAO();
    Texture* ApplyToneMapping(Texture* inputTexture);
    
    // Get configuration
    PostProcessConfig& GetPostProcessConfig() { return m_postProcessConfig; }
    
    // Get G-Buffer textures for debugging
    Texture* GetPositionTexture() { return m_gbuffer.positionTexture; }
    Texture* GetNormalTexture() { return m_gbuffer.normalTexture; }
    Texture* GetAlbedoSpecTexture() { return m_gbuffer.albedoSpecTexture; }
    
    // Shadow mapping
    void BeginShadowMapPass();
    void EndShadowMapPass();
    Texture* GetShadowMap() { return m_shadowMap; }
    
    // Get screen quad mesh
    Mesh* GetScreenQuadMesh() { return m_screenQuad; }
    
private:
    IGraphicsBackend* m_backend;
    ShaderManager* m_shaderManager;
    
    int m_width;
    int m_height;
    
    // Deferred rendering
    GBuffer m_gbuffer;
    std::vector<Light> m_lights;
    
    // Shadow mapping
    Framebuffer* m_shadowMapFBO;
    Texture* m_shadowMap;
    int m_shadowMapSize;
    
    // Post-processing
    PostProcessConfig m_postProcessConfig;
    Framebuffer* m_hdrFBO;
    Texture* m_hdrTexture;
    Framebuffer* m_bloomFBO[2]; // Ping-pong buffers
    Texture* m_bloomTexture[2];
    Framebuffer* m_ssaoFBO;
    Texture* m_ssaoTexture;
    Texture* m_ssaoNoise;
    std::vector<float> m_ssaoKernel;
    
    // Screen quad for post-processing
    Mesh* m_screenQuad;
    
    // Helper functions
    Framebuffer* CreateFramebuffer(int width, int height, int numColorAttachments, bool hasDepth);
    void DestroyFramebuffer(Framebuffer* fbo);
    void CreateGBuffer(int width, int height);
    void DestroyGBuffer();
    void CreateScreenQuad();
    void GenerateSSAOKernel();
    Texture* CreateNoiseTexture(int size);
    
    // Gaussian blur helper
    void GaussianBlur(Texture* inputTexture, Framebuffer* outputFBO, int passes);
};

} // namespace Backend
} // namespace Gfx

#endif // __GFX_BACKEND_MODERN_RENDERER_H

// Texture Enhancement Integration
// Integrates ML-based graphics enhancement with the texture loading pipeline

#ifndef __GFX_ML_TEXTURE_ENHANCER_H
#define __GFX_ML_TEXTURE_ENHANCER_H

#include "GraphicsEnhancer.h"

// Include backend if available
#ifdef USE_OPENGL_RENDERER
#include <Gfx/Backend/GraphicsBackend.h>
#endif

namespace Gfx
{
namespace ML
{

// Texture enhancement configuration
struct TextureEnhanceConfig
{
    bool enableUpscaling;        // Enable automatic texture upscaling
    bool enableSharpening;       // Enable edge sharpening
    bool enableColorEnhance;     // Enable color vibrancy enhancement
    bool enableDenoise;          // Enable noise reduction
    
    int upscaleFactor;           // 2 or 4x upscaling
    float sharpenStrength;       // 0.0 to 1.0
    float colorEnhanceStrength;  // 0.0 to 1.0
    float denoiseStrength;       // 0.0 to 1.0
    
    int minTextureSize;          // Only enhance textures below this size
    int maxTextureSize;          // Don't enhance textures above this size
    
    TextureEnhanceConfig()
        : enableUpscaling(true)
        , enableSharpening(true)
        , enableColorEnhance(false)
        , enableDenoise(false)
        , upscaleFactor(2)
        , sharpenStrength(0.7f)
        , colorEnhanceStrength(0.5f)
        , denoiseStrength(0.6f)
        , minTextureSize(0)
        , maxTextureSize(512)
    {}
};

// Texture enhancement statistics
struct TextureEnhanceStats
{
    int texturesProcessed;
    int texturesUpscaled;
    int texturesSharpened;
    int texturesColorEnhanced;
    int texturesDenoised;
    
    float totalProcessingTimeMs;
    float avgProcessingTimeMs;
    
    TextureEnhanceStats()
        : texturesProcessed(0)
        , texturesUpscaled(0)
        , texturesSharpened(0)
        , texturesColorEnhanced(0)
        , texturesDenoised(0)
        , totalProcessingTimeMs(0.0f)
        , avgProcessingTimeMs(0.0f)
    {}
};

// Texture Enhancer Manager
class CTextureEnhancer
{
public:
    CTextureEnhancer();
    ~CTextureEnhancer();
    
    // Initialize the texture enhancer
    bool Initialize(const TextureEnhanceConfig& config);
    
    // Shutdown and cleanup
    void Shutdown();
    
    // Enhance a texture (RGBA8 format)
    // Returns true if enhancement was applied, false if texture was unchanged
    bool EnhanceTexture(uint8* textureData, int& width, int& height, int channels,
                       uint8** enhancedData, int& enhancedWidth, int& enhancedHeight);
    
    // Get/Set configuration
    const TextureEnhanceConfig& GetConfig() const { return m_config; }
    void SetConfig(const TextureEnhanceConfig& config);
    
    // Get enhancement statistics
    const TextureEnhanceStats& GetStats() const { return m_stats; }
    void ResetStats();
    
    // Check if enhancer is ready
    bool IsReady() const { return m_initialized; }
    
    // Enable/disable enhancement at runtime
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
private:
    bool m_initialized;
    bool m_enabled;
    TextureEnhanceConfig m_config;
    TextureEnhanceStats m_stats;
    
    // Enhancement instances
    IGraphicsEnhancer* m_upscaler2x;
    IGraphicsEnhancer* m_upscaler4x;
    IGraphicsEnhancer* m_sharpener;
    IGraphicsEnhancer* m_colorEnhancer;
    IGraphicsEnhancer* m_denoiser;
    
    // Helper methods
    bool ShouldEnhanceTexture(int width, int height) const;
    void UpdateStats(float processingTimeMs);
};

// Global texture enhancer instance
CTextureEnhancer* GetTextureEnhancer();
void SetTextureEnhancer(CTextureEnhancer* enhancer);

// Utility function to enhance texture during loading
// This is the primary integration point for the texture loading pipeline
bool EnhanceTextureOnLoad(uint8* textureData, int& width, int& height, int channels,
                          uint8** enhancedData, int& enhancedWidth, int& enhancedHeight);

} // namespace ML
} // namespace Gfx

#endif // __GFX_ML_TEXTURE_ENHANCER_H

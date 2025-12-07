// Texture Enhancement Integration Implementation

#include "TextureEnhancer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace Gfx
{
namespace ML
{

// Global texture enhancer instance
static CTextureEnhancer* s_pTextureEnhancer = nullptr;

CTextureEnhancer::CTextureEnhancer()
    : m_initialized(false)
    , m_enabled(true)
    , m_config()
    , m_stats()
    , m_upscaler2x(nullptr)
    , m_upscaler4x(nullptr)
    , m_sharpener(nullptr)
    , m_colorEnhancer(nullptr)
    , m_denoiser(nullptr)
{
}

CTextureEnhancer::~CTextureEnhancer()
{
    Shutdown();
}

bool CTextureEnhancer::Initialize(const TextureEnhanceConfig& config)
{
    if (m_initialized)
    {
        printf("TextureEnhancer: Already initialized\n");
        return true;
    }
    
    printf("TextureEnhancer: Initializing ML-based texture enhancement...\n");
    m_config = config;
    
    // Create enhancers based on configuration
    if (m_config.enableUpscaling)
    {
        if (m_config.upscaleFactor == 2)
        {
            m_upscaler2x = CreateEnhancer(ENHANCEMENT_UPSCALE_2X);
            if (!m_upscaler2x)
            {
                printf("TextureEnhancer: Failed to create 2x upscaler\n");
                return false;
            }
            printf("  ✓ 2x upscaler ready\n");
        }
        else if (m_config.upscaleFactor == 4)
        {
            m_upscaler4x = CreateEnhancer(ENHANCEMENT_UPSCALE_4X);
            if (!m_upscaler4x)
            {
                printf("TextureEnhancer: Failed to create 4x upscaler\n");
                return false;
            }
            printf("  ✓ 4x upscaler ready\n");
        }
    }
    
    if (m_config.enableSharpening)
    {
        m_sharpener = CreateEnhancer(ENHANCEMENT_SHARPEN);
        if (!m_sharpener)
        {
            printf("TextureEnhancer: Failed to create sharpener\n");
            return false;
        }
        printf("  ✓ Sharpener ready (strength: %.2f)\n", m_config.sharpenStrength);
    }
    
    if (m_config.enableColorEnhance)
    {
        m_colorEnhancer = CreateEnhancer(ENHANCEMENT_COLOR_ENHANCE);
        if (!m_colorEnhancer)
        {
            printf("TextureEnhancer: Failed to create color enhancer\n");
            return false;
        }
        printf("  ✓ Color enhancer ready (strength: %.2f)\n", m_config.colorEnhanceStrength);
    }
    
    if (m_config.enableDenoise)
    {
        m_denoiser = CreateEnhancer(ENHANCEMENT_DENOISE);
        if (!m_denoiser)
        {
            printf("TextureEnhancer: Failed to create denoiser\n");
            return false;
        }
        printf("  ✓ Denoiser ready (strength: %.2f)\n", m_config.denoiseStrength);
    }
    
    printf("TextureEnhancer: Initialization complete\n");
    printf("  Texture size range: %dx%d to %dx%d\n", 
           m_config.minTextureSize, m_config.minTextureSize,
           m_config.maxTextureSize, m_config.maxTextureSize);
    
    m_initialized = true;
    return true;
}

void CTextureEnhancer::Shutdown()
{
    if (!m_initialized)
        return;
    
    printf("TextureEnhancer: Shutting down...\n");
    
    if (m_upscaler2x)
    {
        m_upscaler2x->Shutdown();
        delete m_upscaler2x;
        m_upscaler2x = nullptr;
    }
    
    if (m_upscaler4x)
    {
        m_upscaler4x->Shutdown();
        delete m_upscaler4x;
        m_upscaler4x = nullptr;
    }
    
    if (m_sharpener)
    {
        m_sharpener->Shutdown();
        delete m_sharpener;
        m_sharpener = nullptr;
    }
    
    if (m_colorEnhancer)
    {
        m_colorEnhancer->Shutdown();
        delete m_colorEnhancer;
        m_colorEnhancer = nullptr;
    }
    
    if (m_denoiser)
    {
        m_denoiser->Shutdown();
        delete m_denoiser;
        m_denoiser = nullptr;
    }
    
    printf("TextureEnhancer: Shutdown complete\n");
    printf("  Total textures processed: %d\n", m_stats.texturesProcessed);
    printf("  Total processing time: %.2f ms\n", m_stats.totalProcessingTimeMs);
    if (m_stats.texturesProcessed > 0)
    {
        printf("  Average processing time: %.2f ms per texture\n", m_stats.avgProcessingTimeMs);
    }
    
    m_initialized = false;
}

bool CTextureEnhancer::ShouldEnhanceTexture(int width, int height) const
{
    // Check size constraints
    int maxDim = (width > height) ? width : height;
    
    if (maxDim < m_config.minTextureSize)
        return false;
    
    if (maxDim > m_config.maxTextureSize)
        return false;
    
    return true;
}

bool CTextureEnhancer::EnhanceTexture(uint8* textureData, int& width, int& height, int channels,
                                     uint8** enhancedData, int& enhancedWidth, int& enhancedHeight)
{
    if (!m_initialized || !m_enabled || !textureData)
        return false;
    
    if (!ShouldEnhanceTexture(width, height))
        return false;
    
    clock_t startTime = clock();
    bool enhanced = false;
    
    // Store original dimensions
    int origWidth = width;
    int origHeight = height;
    
    // Convert input to float
    ImageData input;
    input.width = origWidth;
    input.height = origHeight;
    input.channels = channels;
    input.data = nullptr;
    Utils::ConvertToFloat(textureData, origWidth, origHeight, &input);
    
    ImageData current = input;
    current.data = new float[input.width * input.height * input.channels];
    memcpy(current.data, input.data, input.width * input.height * input.channels * sizeof(float));
    
    // Apply denoising first if enabled
    if (m_config.enableDenoise && m_denoiser && m_denoiser->IsReady())
    {
        ImageData denoised;
        denoised.data = nullptr;
        
        if (m_denoiser->Enhance(current, denoised))
        {
            delete[] current.data;
            current = denoised;
            m_stats.texturesDenoised++;
            enhanced = true;
        }
    }
    
    // Apply upscaling if enabled
    if (m_config.enableUpscaling)
    {
        IGraphicsEnhancer* upscaler = (m_config.upscaleFactor == 2) ? m_upscaler2x : m_upscaler4x;
        
        if (upscaler && upscaler->IsReady())
        {
            ImageData upscaled;
            upscaled.data = nullptr;
            
            if (upscaler->Enhance(current, upscaled))
            {
                delete[] current.data;
                current = upscaled;
                m_stats.texturesUpscaled++;
                enhanced = true;
            }
        }
    }
    
    // Apply sharpening if enabled
    if (m_config.enableSharpening && m_sharpener && m_sharpener->IsReady())
    {
        ImageData sharpened;
        sharpened.data = nullptr;
        
        if (m_sharpener->Enhance(current, sharpened))
        {
            delete[] current.data;
            current = sharpened;
            m_stats.texturesSharpened++;
            enhanced = true;
        }
    }
    
    // Apply color enhancement if enabled
    if (m_config.enableColorEnhance && m_colorEnhancer && m_colorEnhancer->IsReady())
    {
        ImageData colorEnhanced;
        colorEnhanced.data = nullptr;
        
        if (m_colorEnhancer->Enhance(current, colorEnhanced))
        {
            delete[] current.data;
            current = colorEnhanced;
            m_stats.texturesColorEnhanced++;
            enhanced = true;
        }
    }
    
    // Convert result back to RGBA8
    if (enhanced)
    {
        enhancedWidth = current.width;
        enhancedHeight = current.height;
        
        *enhancedData = new uint8[current.width * current.height * current.channels];
        Utils::ConvertToRGBA8(&current, *enhancedData);
        
        width = enhancedWidth;
        height = enhancedHeight;
    }
    
    // Cleanup
    delete[] input.data;
    delete[] current.data;
    
    // Update statistics
    if (enhanced)
    {
        clock_t endTime = clock();
        float processingTimeMs = ((float)(endTime - startTime) / CLOCKS_PER_SEC) * 1000.0f;
        UpdateStats(processingTimeMs);
    }
    
    return enhanced;
}

void CTextureEnhancer::SetConfig(const TextureEnhanceConfig& config)
{
    m_config = config;
}

void CTextureEnhancer::ResetStats()
{
    m_stats = TextureEnhanceStats();
}

void CTextureEnhancer::UpdateStats(float processingTimeMs)
{
    m_stats.texturesProcessed++;
    m_stats.totalProcessingTimeMs += processingTimeMs;
    m_stats.avgProcessingTimeMs = m_stats.totalProcessingTimeMs / m_stats.texturesProcessed;
}

// Global functions
CTextureEnhancer* GetTextureEnhancer()
{
    return s_pTextureEnhancer;
}

void SetTextureEnhancer(CTextureEnhancer* enhancer)
{
    s_pTextureEnhancer = enhancer;
}

bool EnhanceTextureOnLoad(uint8* textureData, int& width, int& height, int channels,
                          uint8** enhancedData, int& enhancedWidth, int& enhancedHeight)
{
    CTextureEnhancer* enhancer = GetTextureEnhancer();
    if (!enhancer)
        return false;
    
    return enhancer->EnhanceTexture(textureData, width, height, channels,
                                   enhancedData, enhancedWidth, enhancedHeight);
}

} // namespace ML
} // namespace Gfx

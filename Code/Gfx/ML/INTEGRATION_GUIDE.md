# ML Graphics Enhancement - Integration Guide

## Quick Integration for Game Developers

This guide shows how to integrate the ML Graphics Enhancement system into your game's rendering pipeline.

## Prerequisites

- THUG engine with Gfx subsystem compiled
- C++11 compiler
- Basic understanding of texture loading

## Integration Steps

### Step 1: Include Headers

```cpp
#include <Gfx/ML/TextureEnhancer.h>
```

### Step 2: Initialize at Startup

Add to your game initialization code:

```cpp
void Game::Initialize()
{
    // ... existing initialization ...
    
    // Initialize ML texture enhancement
    Gfx::ML::CTextureEnhancer* mlEnhancer = new Gfx::ML::CTextureEnhancer();
    
    Gfx::ML::TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.enableSharpening = true;
    config.upscaleFactor = 2;           // 2x upscaling
    config.sharpenStrength = 0.7f;      // Moderate sharpening
    config.maxTextureSize = 512;        // Only enhance small/medium textures
    
    if (mlEnhancer->Initialize(config))
    {
        Gfx::ML::SetTextureEnhancer(mlEnhancer);
        printf("ML texture enhancement enabled\n");
    }
    else
    {
        delete mlEnhancer;
        printf("ML texture enhancement failed to initialize\n");
    }
}
```

### Step 3: Modify Texture Loading

#### Option A: Automatic Enhancement (Recommended)

Modify your texture loading function:

```cpp
// BEFORE: Without ML enhancement
Texture* LoadTexture(const char* filename)
{
    int width, height;
    uint8* data = LoadImageFile(filename, &width, &height);
    Texture* tex = CreateGPUTexture(data, width, height);
    delete[] data;
    return tex;
}

// AFTER: With ML enhancement
Texture* LoadTexture(const char* filename)
{
    int width, height;
    uint8* data = LoadImageFile(filename, &width, &height);
    
    // Try ML enhancement
    uint8* enhancedData = nullptr;
    int enhancedW = width, enhancedH = height;
    
    if (Gfx::ML::EnhanceTextureOnLoad(data, width, height, 4,
                                     &enhancedData, enhancedW, enhancedH))
    {
        // Use enhanced texture
        Texture* tex = CreateGPUTexture(enhancedData, enhancedW, enhancedH);
        delete[] enhancedData;
        delete[] data;
        return tex;
    }
    
    // Fallback to original
    Texture* tex = CreateGPUTexture(data, width, height);
    delete[] data;
    return tex;
}
```

#### Option B: Selective Enhancement

For more control over which textures get enhanced:

```cpp
Texture* LoadTexture(const char* filename, bool allowEnhancement = true)
{
    int width, height;
    uint8* data = LoadImageFile(filename, &width, &height);
    
    // Check if we should enhance this texture
    bool shouldEnhance = allowEnhancement;
    
    // Only enhance character and UI textures
    if (strstr(filename, "character") || strstr(filename, "ui"))
    {
        shouldEnhance = true;
    }
    // Don't enhance skybox textures
    else if (strstr(filename, "sky"))
    {
        shouldEnhance = false;
    }
    
    if (shouldEnhance)
    {
        uint8* enhanced = nullptr;
        int enhW = width, enhH = height;
        
        if (Gfx::ML::EnhanceTextureOnLoad(data, width, height, 4,
                                         &enhanced, enhW, enhH))
        {
            Texture* tex = CreateGPUTexture(enhanced, enhW, enhH);
            delete[] enhanced;
            delete[] data;
            return tex;
        }
    }
    
    Texture* tex = CreateGPUTexture(data, width, height);
    delete[] data;
    return tex;
}
```

### Step 4: Configuration Options (Optional)

Allow users to configure enhancement quality:

```cpp
enum QualityLevel
{
    QUALITY_LOW,
    QUALITY_MEDIUM,
    QUALITY_HIGH,
    QUALITY_ULTRA
};

void SetEnhancementQuality(QualityLevel quality)
{
    Gfx::ML::CTextureEnhancer* enhancer = Gfx::ML::GetTextureEnhancer();
    if (!enhancer) return;
    
    Gfx::ML::TextureEnhanceConfig config;
    
    switch (quality)
    {
        case QUALITY_LOW:
            config.enableUpscaling = true;
            config.enableSharpening = false;
            config.upscaleFactor = 2;
            config.maxTextureSize = 256;
            break;
            
        case QUALITY_MEDIUM:
            config.enableUpscaling = true;
            config.enableSharpening = true;
            config.upscaleFactor = 2;
            config.sharpenStrength = 0.5f;
            config.maxTextureSize = 512;
            break;
            
        case QUALITY_HIGH:
            config.enableUpscaling = true;
            config.enableSharpening = true;
            config.upscaleFactor = 2;
            config.sharpenStrength = 0.8f;
            config.maxTextureSize = 1024;
            break;
            
        case QUALITY_ULTRA:
            config.enableUpscaling = true;
            config.enableSharpening = true;
            config.enableColorEnhance = true;
            config.upscaleFactor = 2;
            config.sharpenStrength = 0.9f;
            config.colorEnhanceStrength = 0.6f;
            config.maxTextureSize = 1024;
            break;
    }
    
    enhancer->SetConfig(config);
}
```

### Step 5: Runtime Enable/Disable

Allow users to toggle enhancement:

```cpp
void ToggleMLEnhancement(bool enabled)
{
    Gfx::ML::CTextureEnhancer* enhancer = Gfx::ML::GetTextureEnhancer();
    if (enhancer)
    {
        enhancer->SetEnabled(enabled);
        printf("ML enhancement %s\n", enabled ? "enabled" : "disabled");
    }
}
```

### Step 6: Cleanup on Shutdown

```cpp
void Game::Shutdown()
{
    // Cleanup ML enhancement
    Gfx::ML::CTextureEnhancer* enhancer = Gfx::ML::GetTextureEnhancer();
    if (enhancer)
    {
        // Print statistics
        const Gfx::ML::TextureEnhanceStats& stats = enhancer->GetStats();
        printf("ML Enhancement Statistics:\n");
        printf("  Textures processed: %d\n", stats.texturesProcessed);
        printf("  Average time: %.2f ms\n", stats.avgProcessingTimeMs);
        
        enhancer->Shutdown();
        delete enhancer;
        Gfx::ML::SetTextureEnhancer(nullptr);
    }
    
    // ... existing cleanup ...
}
```

## Example: Complete Integration

Here's a complete example showing integration with a typical texture manager:

```cpp
// TextureManager.h
class CTextureManager
{
public:
    void Initialize();
    void Shutdown();
    
    Texture* LoadTexture(const char* filename);
    void SetMLQuality(int quality);
    void EnableML(bool enable);
    
private:
    Gfx::ML::CTextureEnhancer* m_mlEnhancer;
};

// TextureManager.cpp
#include "TextureManager.h"
#include <Gfx/ML/TextureEnhancer.h>

void CTextureManager::Initialize()
{
    // Initialize ML enhancement
    m_mlEnhancer = new Gfx::ML::CTextureEnhancer();
    
    Gfx::ML::TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.enableSharpening = true;
    config.upscaleFactor = 2;
    config.maxTextureSize = 512;
    
    if (m_mlEnhancer->Initialize(config))
    {
        Gfx::ML::SetTextureEnhancer(m_mlEnhancer);
        printf("ML texture enhancement initialized\n");
    }
    else
    {
        delete m_mlEnhancer;
        m_mlEnhancer = nullptr;
    }
}

void CTextureManager::Shutdown()
{
    if (m_mlEnhancer)
    {
        m_mlEnhancer->Shutdown();
        delete m_mlEnhancer;
        m_mlEnhancer = nullptr;
        Gfx::ML::SetTextureEnhancer(nullptr);
    }
}

Texture* CTextureManager::LoadTexture(const char* filename)
{
    // Load image data
    int width, height, channels;
    uint8* data = LoadImageFile(filename, &width, &height, &channels);
    if (!data) return nullptr;
    
    // Try ML enhancement
    uint8* finalData = data;
    int finalWidth = width;
    int finalHeight = height;
    
    if (m_mlEnhancer && m_mlEnhancer->IsEnabled())
    {
        uint8* enhanced = nullptr;
        int enhW = width, enhH = height;
        
        if (Gfx::ML::EnhanceTextureOnLoad(data, width, height, channels,
                                         &enhanced, enhW, enhH))
        {
            finalData = enhanced;
            finalWidth = enhW;
            finalHeight = enhH;
        }
    }
    
    // Create GPU texture
    Texture* tex = CreateGPUTexture(finalData, finalWidth, finalHeight, channels);
    
    // Cleanup
    if (finalData != data)
        delete[] finalData;
    delete[] data;
    
    return tex;
}

void CTextureManager::SetMLQuality(int quality)
{
    if (!m_mlEnhancer) return;
    
    Gfx::ML::TextureEnhanceConfig config;
    
    // Configure based on quality level (0-3)
    config.enableUpscaling = true;
    config.enableSharpening = (quality >= 1);
    config.enableColorEnhance = (quality >= 3);
    config.upscaleFactor = 2;
    config.sharpenStrength = 0.5f + quality * 0.1f;
    config.maxTextureSize = 256 * (1 << quality); // 256, 512, 1024, 2048
    
    m_mlEnhancer->SetConfig(config);
}

void CTextureManager::EnableML(bool enable)
{
    if (m_mlEnhancer)
    {
        m_mlEnhancer->SetEnabled(enable);
    }
}
```

## Performance Recommendations

### For Best Performance

1. **Set size limits**: `maxTextureSize = 512` or lower
2. **Disable for large textures**: Skip textures > 512×512
3. **Use async loading**: Process textures on background thread
4. **Cache results**: Save enhanced textures to disk

### For Best Quality

1. **Enable all effects**: Upscaling + Sharpening + Color Enhancement
2. **Higher strength**: `sharpenStrength = 0.8-0.9`
3. **Larger size limit**: `maxTextureSize = 1024`
4. **Consider 4x upscaling**: For very low-res textures

## Troubleshooting

### Enhancement Not Working

1. Check if enhancer is initialized:
```cpp
Gfx::ML::CTextureEnhancer* e = Gfx::ML::GetTextureEnhancer();
printf("Enhancer ready: %d\n", e ? e->IsReady() : 0);
```

2. Check if enabled:
```cpp
printf("Enhancer enabled: %d\n", e ? e->IsEnabled() : 0);
```

3. Check texture size:
```cpp
const auto& config = e->GetConfig();
printf("Size range: %d-%d\n", config.minTextureSize, config.maxTextureSize);
```

### Memory Issues

If running out of memory:

1. Lower `maxTextureSize` to 256 or 128
2. Process textures sequentially, not in parallel
3. Ensure enhanced data is freed after GPU upload

### Performance Issues

If loading is too slow:

1. Reduce `maxTextureSize`
2. Disable sharpening and color enhancement
3. Move enhancement to background thread
4. Cache enhanced textures

## Complete Example Project

See `Code/Gfx/Examples/integrated_ml_demo.cpp` for a complete working example showing:

- Scene loading with ML enhancement
- Quality comparisons
- Performance profiling
- Configurable quality levels
- Memory usage analysis

## Next Steps

1. Test with actual game assets
2. Tune configuration for your specific needs
3. Implement texture caching if needed
4. Consider GPU acceleration for production

## Support

For issues or questions, see:
- `Code/Gfx/ML/README.md` - Full API documentation
- `Code/Gfx/Examples/` - Working examples
- THUG repository issues on GitHub

---

*Last updated: December 2024*

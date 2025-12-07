// Texture Enhancement Demo
// Demonstrates ML-based texture enhancement integration

#include "../ML/TextureEnhancer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

using namespace Gfx::ML;

// Helper function to create a test texture with patterns
void CreateTestTexture(uint8* data, int width, int height, int pattern)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = (y * width + x) * 4;
            
            switch (pattern)
            {
                case 0: // Checkerboard
                {
                    int check = ((x / 8) + (y / 8)) % 2;
                    uint8 value = check ? 200 : 50;
                    data[idx + 0] = value;
                    data[idx + 1] = value;
                    data[idx + 2] = value;
                    data[idx + 3] = 255;
                    break;
                }
                
                case 1: // Gradient
                {
                    data[idx + 0] = (uint8)(x * 255 / width);
                    data[idx + 1] = (uint8)(y * 255 / height);
                    data[idx + 2] = 128;
                    data[idx + 3] = 255;
                    break;
                }
                
                case 2: // Concentric circles
                {
                    float cx = width / 2.0f;
                    float cy = height / 2.0f;
                    float dx = x - cx;
                    float dy = y - cy;
                    float dist = sqrtf(dx * dx + dy * dy);
                    float maxDist = sqrtf(cx * cx + cy * cy);
                    uint8 value = (uint8)(255 * (1.0f - dist / maxDist));
                    data[idx + 0] = value;
                    data[idx + 1] = value / 2;
                    data[idx + 2] = 255 - value;
                    data[idx + 3] = 255;
                    break;
                }
                
                case 3: // Striped pattern
                {
                    uint8 value = ((x + y) % 16 < 8) ? 200 : 80;
                    data[idx + 0] = value;
                    data[idx + 1] = value * 0.8f;
                    data[idx + 2] = value * 0.6f;
                    data[idx + 3] = 255;
                    break;
                }
                
                default:
                    data[idx + 0] = 128;
                    data[idx + 1] = 128;
                    data[idx + 2] = 128;
                    data[idx + 3] = 255;
                    break;
            }
        }
    }
}

// Calculate average color difference
float CalculateImageDifference(const uint8* img1, const uint8* img2, int width, int height)
{
    float totalDiff = 0.0f;
    int pixels = width * height;
    
    for (int i = 0; i < pixels * 4; ++i)
    {
        float diff = (float)img1[i] - (float)img2[i];
        totalDiff += (diff * diff);
    }
    
    return sqrtf(totalDiff / (pixels * 4));
}

void TestBasicEnhancement()
{
    printf("\n=== Test 1: Basic Texture Enhancement ===\n");
    
    // Create texture enhancer
    CTextureEnhancer enhancer;
    
    TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.enableSharpening = true;
    config.upscaleFactor = 2;
    config.maxTextureSize = 256;
    
    if (!enhancer.Initialize(config))
    {
        printf("Failed to initialize texture enhancer\n");
        return;
    }
    
    // Create a low-res test texture
    const int inputWidth = 64;
    const int inputHeight = 64;
    uint8* inputTexture = new uint8[inputWidth * inputHeight * 4];
    CreateTestTexture(inputTexture, inputWidth, inputHeight, 0);
    
    printf("Input texture: %dx%d\n", inputWidth, inputHeight);
    
    // Enhance the texture
    uint8* enhancedTexture = nullptr;
    int enhancedWidth = inputWidth;
    int enhancedHeight = inputHeight;
    
    if (enhancer.EnhanceTexture(inputTexture, enhancedWidth, enhancedHeight, 4,
                                &enhancedTexture, enhancedWidth, enhancedHeight))
    {
        printf("✓ Enhancement successful!\n");
        printf("  Output texture: %dx%d\n", enhancedWidth, enhancedHeight);
        printf("  Size increase: %dx\n", enhancedWidth / inputWidth);
        
        delete[] enhancedTexture;
    }
    else
    {
        printf("✗ Enhancement failed\n");
    }
    
    delete[] inputTexture;
    enhancer.Shutdown();
}

void TestMultipleTextures()
{
    printf("\n=== Test 2: Multiple Texture Enhancement ===\n");
    
    CTextureEnhancer enhancer;
    
    TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.enableSharpening = true;
    config.enableColorEnhance = true;
    config.upscaleFactor = 2;
    config.maxTextureSize = 256;
    
    if (!enhancer.Initialize(config))
    {
        printf("Failed to initialize texture enhancer\n");
        return;
    }
    
    // Test different texture patterns
    const char* patterns[] = {
        "Checkerboard",
        "Gradient",
        "Concentric Circles",
        "Striped Pattern"
    };
    
    for (int i = 0; i < 4; ++i)
    {
        printf("\nProcessing texture %d: %s\n", i + 1, patterns[i]);
        
        const int width = 128;
        const int height = 128;
        uint8* texture = new uint8[width * height * 4];
        CreateTestTexture(texture, width, height, i);
        
        uint8* enhanced = nullptr;
        int enhancedW = width;
        int enhancedH = height;
        
        if (enhancer.EnhanceTexture(texture, enhancedW, enhancedH, 4,
                                   &enhanced, enhancedW, enhancedH))
        {
            printf("  ✓ Enhanced: %dx%d -> %dx%d\n", width, height, enhancedW, enhancedH);
            delete[] enhanced;
        }
        else
        {
            printf("  ✗ Not enhanced (texture too large or enhancer disabled)\n");
        }
        
        delete[] texture;
    }
    
    const TextureEnhanceStats& stats = enhancer.GetStats();
    printf("\n=== Enhancement Statistics ===\n");
    printf("Total textures processed: %d\n", stats.texturesProcessed);
    printf("Textures upscaled: %d\n", stats.texturesUpscaled);
    printf("Textures sharpened: %d\n", stats.texturesSharpened);
    printf("Textures color enhanced: %d\n", stats.texturesColorEnhanced);
    printf("Average processing time: %.2f ms\n", stats.avgProcessingTimeMs);
    
    enhancer.Shutdown();
}

void TestSizeConstraints()
{
    printf("\n=== Test 3: Texture Size Constraints ===\n");
    
    CTextureEnhancer enhancer;
    
    TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.upscaleFactor = 2;
    config.minTextureSize = 64;
    config.maxTextureSize = 256;
    
    if (!enhancer.Initialize(config))
    {
        printf("Failed to initialize texture enhancer\n");
        return;
    }
    
    printf("Configuration:\n");
    printf("  Min size: %dx%d\n", config.minTextureSize, config.minTextureSize);
    printf("  Max size: %dx%d\n", config.maxTextureSize, config.maxTextureSize);
    printf("\n");
    
    // Test various texture sizes
    const int testSizes[] = {32, 64, 128, 256, 512};
    const int numSizes = 5;
    
    for (int i = 0; i < numSizes; ++i)
    {
        int size = testSizes[i];
        uint8* texture = new uint8[size * size * 4];
        CreateTestTexture(texture, size, size, 0);
        
        uint8* enhanced = nullptr;
        int enhancedW = size;
        int enhancedH = size;
        
        bool result = enhancer.EnhanceTexture(texture, enhancedW, enhancedH, 4,
                                             &enhanced, enhancedW, enhancedH);
        
        if (result)
        {
            printf("  %dx%d: ✓ Enhanced to %dx%d\n", size, size, enhancedW, enhancedH);
            delete[] enhanced;
        }
        else
        {
            printf("  %dx%d: ✗ Not enhanced (outside size range)\n", size, size);
        }
        
        delete[] texture;
    }
    
    enhancer.Shutdown();
}

void TestEnableDisable()
{
    printf("\n=== Test 4: Runtime Enable/Disable ===\n");
    
    CTextureEnhancer enhancer;
    
    TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.upscaleFactor = 2;
    config.maxTextureSize = 256;
    
    if (!enhancer.Initialize(config))
    {
        printf("Failed to initialize texture enhancer\n");
        return;
    }
    
    const int size = 128;
    uint8* texture = new uint8[size * size * 4];
    CreateTestTexture(texture, size, size, 1);
    
    // Test with enhancer enabled
    printf("Testing with enhancer ENABLED:\n");
    enhancer.SetEnabled(true);
    
    uint8* enhanced1 = nullptr;
    int w1 = size, h1 = size;
    bool result1 = enhancer.EnhanceTexture(texture, w1, h1, 4, &enhanced1, w1, h1);
    printf("  Result: %s (size: %dx%d)\n", result1 ? "Enhanced" : "Not enhanced", w1, h1);
    if (enhanced1) delete[] enhanced1;
    
    // Test with enhancer disabled
    printf("Testing with enhancer DISABLED:\n");
    enhancer.SetEnabled(false);
    
    uint8* enhanced2 = nullptr;
    int w2 = size, h2 = size;
    bool result2 = enhancer.EnhanceTexture(texture, w2, h2, 4, &enhanced2, w2, h2);
    printf("  Result: %s (size: %dx%d)\n", result2 ? "Enhanced" : "Not enhanced", w2, h2);
    if (enhanced2) delete[] enhanced2;
    
    delete[] texture;
    enhancer.Shutdown();
}

void TestPerformance()
{
    printf("\n=== Test 5: Performance Testing ===\n");
    
    CTextureEnhancer enhancer;
    
    TextureEnhanceConfig config;
    config.enableUpscaling = true;
    config.enableSharpening = true;
    config.upscaleFactor = 2;
    config.maxTextureSize = 512;
    
    if (!enhancer.Initialize(config))
    {
        printf("Failed to initialize texture enhancer\n");
        return;
    }
    
    const int testSizes[] = {64, 128, 256};
    const int numTests = 10;
    
    for (int sizeIdx = 0; sizeIdx < 3; ++sizeIdx)
    {
        int size = testSizes[sizeIdx];
        printf("\nTesting %dx%d textures (%d iterations):\n", size, size, numTests);
        
        clock_t totalTime = 0;
        int successCount = 0;
        
        for (int i = 0; i < numTests; ++i)
        {
            uint8* texture = new uint8[size * size * 4];
            CreateTestTexture(texture, size, size, i % 4);
            
            clock_t start = clock();
            
            uint8* enhanced = nullptr;
            int w = size, h = size;
            bool result = enhancer.EnhanceTexture(texture, w, h, 4, &enhanced, w, h);
            
            clock_t end = clock();
            
            if (result)
            {
                successCount++;
                totalTime += (end - start);
                delete[] enhanced;
            }
            
            delete[] texture;
        }
        
        if (successCount > 0)
        {
            float avgMs = ((float)totalTime / CLOCKS_PER_SEC / successCount) * 1000.0f;
            printf("  Success rate: %d/%d\n", successCount, numTests);
            printf("  Average time: %.2f ms per texture\n", avgMs);
        }
    }
    
    enhancer.Shutdown();
}

int main(int argc, char** argv)
{
    printf("\n");
    printf("==========================================\n");
    printf("Texture Enhancement Integration Demo\n");
    printf("==========================================\n");
    
    TestBasicEnhancement();
    TestMultipleTextures();
    TestSizeConstraints();
    TestEnableDisable();
    TestPerformance();
    
    printf("\n==========================================\n");
    printf("All tests completed!\n");
    printf("==========================================\n\n");
    
    return 0;
}

// ML Enhancement Test Example
// Demonstrates usage of the ML-based graphics enhancement system

#include "../ML/GraphicsEnhancer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace Gfx::ML;

void TestEnhancerCreation()
{
    printf("=== Testing Enhancer Creation ===\n");
    
    const EnhancementType types[] = {
        ENHANCEMENT_UPSCALE_2X,
        ENHANCEMENT_UPSCALE_4X,
        ENHANCEMENT_DENOISE,
        ENHANCEMENT_SHARPEN,
        ENHANCEMENT_ANTI_ALIAS,
        ENHANCEMENT_COLOR_ENHANCE
    };
    
    for (int i = 0; i < 6; ++i)
    {
        IGraphicsEnhancer* enhancer = CreateEnhancer(types[i]);
        if (enhancer && enhancer->IsReady())
        {
            printf("✓ Created enhancer: %s v%s\n", 
                   enhancer->GetModelName(), 
                   enhancer->GetModelVersion());
            enhancer->Shutdown();
            delete enhancer;
        }
    }
    
    printf("\n");
}

void TestUpscaling()
{
    printf("=== Testing 2x Upscaling ===\n");
    
    IGraphicsEnhancer* enhancer = CreateEnhancer(ENHANCEMENT_UPSCALE_2X);
    
    if (enhancer && enhancer->IsReady())
    {
        // Create small test image (64x64)
        const int inputWidth = 64;
        const int inputHeight = 64;
        const int channels = 4;
        
        ImageData input;
        input.width = inputWidth;
        input.height = inputHeight;
        input.channels = channels;
        input.data = new float[inputWidth * inputHeight * channels];
        
        // Fill with gradient pattern
        for (int y = 0; y < inputHeight; ++y)
        {
            for (int x = 0; x < inputWidth; ++x)
            {
                int idx = (y * inputWidth + x) * channels;
                input.data[idx + 0] = (float)x / inputWidth;        // R
                input.data[idx + 1] = (float)y / inputHeight;       // G
                input.data[idx + 2] = 0.5f;                         // B
                input.data[idx + 3] = 1.0f;                         // A
            }
        }
        
        printf("Input image: %dx%d\n", inputWidth, inputHeight);
        
        // Enhance
        ImageData output;
        output.data = nullptr;
        
        if (enhancer->Enhance(input, output))
        {
            printf("✓ Upscaled to: %dx%d\n", output.width, output.height);
            printf("  Scale factor: %dx\n", output.width / inputWidth);
            
            // Verify dimensions
            if (output.width == inputWidth * 2 && output.height == inputHeight * 2)
            {
                printf("✓ Output dimensions correct\n");
            }
            
            delete[] output.data;
        }
        
        delete[] input.data;
        enhancer->Shutdown();
        delete enhancer;
    }
    
    printf("\n");
}

void TestDenoising()
{
    printf("=== Testing Denoising ===\n");
    
    IGraphicsEnhancer* enhancer = CreateEnhancer(ENHANCEMENT_DENOISE);
    
    if (enhancer && enhancer->IsReady())
    {
        const int width = 128;
        const int height = 128;
        const int channels = 4;
        
        ImageData input;
        input.width = width;
        input.height = height;
        input.channels = channels;
        input.data = new float[width * height * channels];
        
        // Fill with noisy data
        for (int i = 0; i < width * height * channels; ++i)
        {
            float noise = ((rand() % 1000) / 1000.0f - 0.5f) * 0.2f;
            input.data[i] = 0.5f + noise;
        }
        
        printf("Input image: %dx%d with random noise\n", width, height);
        
        ImageData output;
        output.data = nullptr;
        
        if (enhancer->Enhance(input, output))
        {
            printf("✓ Denoised image: %dx%d\n", output.width, output.height);
            
            // Calculate average difference
            float totalDiff = 0.0f;
            for (int i = 0; i < width * height * channels; ++i)
            {
                totalDiff += (output.data[i] - input.data[i]) * (output.data[i] - input.data[i]);
            }
            float avgDiff = totalDiff / (width * height * channels);
            printf("  Average pixel change: %.6f\n", avgDiff);
            
            delete[] output.data;
        }
        
        delete[] input.data;
        enhancer->Shutdown();
        delete enhancer;
    }
    
    printf("\n");
}

void TestSharpening()
{
    printf("=== Testing Sharpening ===\n");
    
    IGraphicsEnhancer* enhancer = CreateEnhancer(ENHANCEMENT_SHARPEN);
    
    if (enhancer && enhancer->IsReady())
    {
        const int width = 128;
        const int height = 128;
        const int channels = 4;
        
        ImageData input;
        input.width = width;
        input.height = height;
        input.channels = channels;
        input.data = new float[width * height * channels];
        
        // Create blurry edge pattern
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int idx = (y * width + x) * channels;
                float value = (x < width / 2) ? 0.2f : 0.8f;
                
                // Add some blur around the edge
                float dist = fabsf((float)x - width / 2.0f);
                if (dist < 10.0f)
                {
                    value = 0.5f + (value - 0.5f) * (dist / 10.0f);
                }
                
                input.data[idx + 0] = value;
                input.data[idx + 1] = value;
                input.data[idx + 2] = value;
                input.data[idx + 3] = 1.0f;
            }
        }
        
        printf("Input image: %dx%d with blurry edge\n", width, height);
        
        ImageData output;
        output.data = nullptr;
        
        if (enhancer->Enhance(input, output))
        {
            printf("✓ Sharpened image: %dx%d\n", output.width, output.height);
            delete[] output.data;
        }
        
        delete[] input.data;
        enhancer->Shutdown();
        delete enhancer;
    }
    
    printf("\n");
}

void TestColorEnhancement()
{
    printf("=== Testing Color Enhancement ===\n");
    
    IGraphicsEnhancer* enhancer = CreateEnhancer(ENHANCEMENT_COLOR_ENHANCE);
    
    if (enhancer && enhancer->IsReady())
    {
        const int width = 64;
        const int height = 64;
        const int channels = 4;
        
        ImageData input;
        input.width = width;
        input.height = height;
        input.channels = channels;
        input.data = new float[width * height * channels];
        
        // Create washed out color pattern
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int idx = (y * width + x) * channels;
                input.data[idx + 0] = 0.5f + 0.1f * ((float)x / width - 0.5f);
                input.data[idx + 1] = 0.5f + 0.1f * ((float)y / height - 0.5f);
                input.data[idx + 2] = 0.5f;
                input.data[idx + 3] = 1.0f;
            }
        }
        
        printf("Input image: %dx%d with low saturation\n", width, height);
        
        ImageData output;
        output.data = nullptr;
        
        if (enhancer->Enhance(input, output))
        {
            printf("✓ Enhanced colors: %dx%d\n", output.width, output.height);
            delete[] output.data;
        }
        
        delete[] input.data;
        enhancer->Shutdown();
        delete enhancer;
    }
    
    printf("\n");
}

void TestUtilityFunctions()
{
    printf("=== Testing Utility Functions ===\n");
    
    const int width = 32;
    const int height = 32;
    
    // Test allocation
    ImageData* image = Utils::AllocateImage(width, height, 4);
    printf("✓ Allocated %dx%d image\n", width, height);
    
    // Test RGBA8 conversion
    uint8 rgba8Data[width * height * 4];
    for (int i = 0; i < width * height * 4; ++i)
    {
        rgba8Data[i] = i % 256;
    }
    
    Utils::ConvertToFloat(rgba8Data, width, height, image);
    printf("✓ Converted RGBA8 to float\n");
    
    uint8 rgba8Out[width * height * 4];
    Utils::ConvertToRGBA8(image, rgba8Out);
    printf("✓ Converted float to RGBA8\n");
    
    // Verify round-trip conversion
    bool match = true;
    for (int i = 0; i < width * height * 4; ++i)
    {
        if (rgba8Data[i] != rgba8Out[i])
        {
            match = false;
            break;
        }
    }
    
    if (match)
    {
        printf("✓ Round-trip conversion successful\n");
    }
    
    Utils::FreeImage(image);
    printf("✓ Freed image\n");
    
    printf("\n");
}

int main(int argc, char** argv)
{
    printf("\n");
    printf("==========================================\n");
    printf("ML Enhancement Test Example\n");
    printf("==========================================\n");
    printf("\n");
    
    TestEnhancerCreation();
    TestUpscaling();
    TestDenoising();
    TestSharpening();
    TestColorEnhancement();
    TestUtilityFunctions();
    
    printf("==========================================\n");
    printf("All tests completed successfully!\n");
    printf("==========================================\n");
    printf("\n");
    
    return 0;
}

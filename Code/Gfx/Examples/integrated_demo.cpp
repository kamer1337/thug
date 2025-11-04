// Integrated Backend + ML Example
// Demonstrates combined usage of graphics backend and ML enhancement

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Use standalone mode
#include "../Backend/GraphicsBackend.h"
#include "../ML/GraphicsEnhancer.h"

using namespace Gfx::Backend;
using namespace Gfx::ML;

// Helper to create test texture data
void CreateTestTexture(uint8* data, int width, int height, int channels)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = (y * width + x) * channels;
            // Create checkerboard pattern
            bool isWhite = ((x / 8) + (y / 8)) % 2 == 0;
            uint8 value = isWhite ? 255 : 64;
            
            data[idx + 0] = value;  // R
            data[idx + 1] = value;  // G
            data[idx + 2] = value;  // B
            data[idx + 3] = 255;    // A
        }
    }
}

// Example 1: Basic backend usage
void Example1_BasicBackend()
{
    printf("\n=== Example 1: Basic Backend Usage ===\n");
    
    IGraphicsBackend* backend = CreateBackend("opengl");
    
    if (backend->Initialize())
    {
        printf("✓ Backend: %s %s\n", backend->GetRendererName(), backend->GetVersion());
        
        // Set viewport
        backend->SetViewport(0, 0, 1920, 1080);
        printf("✓ Viewport configured\n");
        
        // Simulate a simple render loop
        for (int frame = 0; frame < 3; ++frame)
        {
            backend->BeginFrame();
            backend->Clear(CLEAR_COLOR | CLEAR_DEPTH, 0x000000FF);
            // ... rendering would happen here ...
            backend->EndFrame();
            backend->SwapBuffers();
        }
        printf("✓ Rendered 3 frames\n");
        
        backend->Shutdown();
    }
    
    delete backend;
}

// Example 2: Texture upscaling with ML
void Example2_TextureUpscaling()
{
    printf("\n=== Example 2: ML Texture Upscaling ===\n");
    
    // Create backend
    IGraphicsBackend* backend = CreateBackend("stub");
    backend->Initialize();
    
    // Create ML enhancer
    IGraphicsEnhancer* enhancer = CreateEnhancer(ENHANCEMENT_UPSCALE_2X);
    
    if (enhancer && enhancer->IsReady())
    {
        // Create low-resolution texture (128x128)
        const int lowResWidth = 128;
        const int lowResHeight = 128;
        const int channels = 4;
        
        uint8* lowResData = new uint8[lowResWidth * lowResHeight * channels];
        CreateTestTexture(lowResData, lowResWidth, lowResHeight, channels);
        
        printf("Created low-res texture: %dx%d\n", lowResWidth, lowResHeight);
        
        // Convert to float for ML processing
        ImageData input;
        input.width = 0;
        input.height = 0;
        input.channels = 0;
        input.data = nullptr;
        Utils::ConvertToFloat(lowResData, lowResWidth, lowResHeight, &input);
        
        // Upscale with ML
        ImageData output;
        output.data = nullptr;
        
        if (enhancer->Enhance(input, output))
        {
            printf("✓ ML upscaled to: %dx%d\n", output.width, output.height);
            
            // Convert back to RGBA8
            uint8* highResData = new uint8[output.width * output.height * channels];
            Utils::ConvertToRGBA8(&output, highResData);
            
            // Create high-res texture in backend
            Texture* highResTex = backend->CreateTexture(output.width, output.height, 
                                                         TEXTURE_FORMAT_RGBA8, highResData);
            
            if (highResTex)
            {
                printf("✓ Created high-res texture in backend\n");
                backend->DestroyTexture(highResTex);
            }
            
            delete[] highResData;
            delete[] output.data;
        }
        
        delete[] lowResData;
        delete[] input.data;
        
        enhancer->Shutdown();
        delete enhancer;
    }
    
    backend->Shutdown();
    delete backend;
}

// Example 3: Real-time ML enhancement pipeline
void Example3_EnhancementPipeline()
{
    printf("\n=== Example 3: Real-time Enhancement Pipeline ===\n");
    
    IGraphicsBackend* backend = CreateBackend("stub");
    backend->Initialize();
    
    // Create multiple enhancers
    IGraphicsEnhancer* upscaler = CreateEnhancer(ENHANCEMENT_UPSCALE_2X);
    IGraphicsEnhancer* sharpener = CreateEnhancer(ENHANCEMENT_SHARPEN);
    IGraphicsEnhancer* colorEnhancer = CreateEnhancer(ENHANCEMENT_COLOR_ENHANCE);
    
    printf("Created enhancement pipeline:\n");
    printf("  1. %s\n", upscaler->GetModelName());
    printf("  2. %s\n", sharpener->GetModelName());
    printf("  3. %s\n", colorEnhancer->GetModelName());
    
    // Create source texture
    const int sourceWidth = 64;
    const int sourceHeight = 64;
    const int channels = 4;
    
    uint8* sourceData = new uint8[sourceWidth * sourceHeight * channels];
    CreateTestTexture(sourceData, sourceWidth, sourceHeight, channels);
    
    // Convert to float
    ImageData stage1, stage2, stage3, stage4;
    stage1.data = nullptr;
    stage2.data = nullptr;
    stage3.data = nullptr;
    stage4.data = nullptr;
    Utils::ConvertToFloat(sourceData, sourceWidth, sourceHeight, &stage1);
    
    printf("\nProcessing pipeline:\n");
    printf("  Stage 1: Input %dx%d\n", stage1.width, stage1.height);
    
    // Stage 2: Upscale
    stage2.data = nullptr;
    if (upscaler->Enhance(stage1, stage2))
    {
        printf("  Stage 2: Upscaled to %dx%d\n", stage2.width, stage2.height);
        
        // Stage 3: Sharpen
        stage3.data = nullptr;
        if (sharpener->Enhance(stage2, stage3))
        {
            printf("  Stage 3: Sharpened %dx%d\n", stage3.width, stage3.height);
            
            // Stage 4: Color enhance
            stage4.data = nullptr;
            if (colorEnhancer->Enhance(stage3, stage4))
            {
                printf("  Stage 4: Color enhanced %dx%d\n", stage4.width, stage4.height);
                
                // Convert back and create texture
                uint8* finalData = new uint8[stage4.width * stage4.height * channels];
                Utils::ConvertToRGBA8(&stage4, finalData);
                
                Texture* finalTex = backend->CreateTexture(stage4.width, stage4.height,
                                                           TEXTURE_FORMAT_RGBA8, finalData);
                
                if (finalTex)
                {
                    printf("✓ Final enhanced texture created\n");
                    backend->DestroyTexture(finalTex);
                }
                
                delete[] finalData;
                delete[] stage4.data;
            }
            delete[] stage3.data;
        }
        delete[] stage2.data;
    }
    
    delete[] stage1.data;
    delete[] sourceData;
    
    // Cleanup
    upscaler->Shutdown();
    sharpener->Shutdown();
    colorEnhancer->Shutdown();
    delete upscaler;
    delete sharpener;
    delete colorEnhancer;
    
    backend->Shutdown();
    delete backend;
}

// Example 4: Performance comparison
void Example4_PerformanceTest()
{
    printf("\n=== Example 4: Performance Characteristics ===\n");
    
    IGraphicsEnhancer* enhancer2x = CreateEnhancer(ENHANCEMENT_UPSCALE_2X);
    IGraphicsEnhancer* enhancer4x = CreateEnhancer(ENHANCEMENT_UPSCALE_4X);
    IGraphicsEnhancer* denoiser = CreateEnhancer(ENHANCEMENT_DENOISE);
    
    printf("\nEnhancement Performance (relative):\n");
    
    // Test with 256x256 image
    const int testWidth = 256;
    const int testHeight = 256;
    
    ImageData testInput;
    testInput.width = testWidth;
    testInput.height = testHeight;
    testInput.channels = 4;
    testInput.data = new float[testWidth * testHeight * 4];
    
    // Fill with dummy data
    for (int i = 0; i < testWidth * testHeight * 4; ++i)
        testInput.data[i] = 0.5f;
    
    // 2x upscaling
    ImageData output2x;
    output2x.data = nullptr;
    if (enhancer2x->Enhance(testInput, output2x))
    {
        printf("  2x Upscale: %dx%d -> %dx%d\n", 
               testInput.width, testInput.height, output2x.width, output2x.height);
        delete[] output2x.data;
    }
    
    // 4x upscaling
    ImageData output4x;
    output4x.data = nullptr;
    if (enhancer4x->Enhance(testInput, output4x))
    {
        printf("  4x Upscale: %dx%d -> %dx%d (4x data to process)\n", 
               testInput.width, testInput.height, output4x.width, output4x.height);
        delete[] output4x.data;
    }
    
    // Denoising
    ImageData outputDenoise;
    outputDenoise.data = nullptr;
    if (denoiser->Enhance(testInput, outputDenoise))
    {
        printf("  Denoise: %dx%d (same size, filtering only)\n", 
               outputDenoise.width, outputDenoise.height);
        delete[] outputDenoise.data;
    }
    
    delete[] testInput.data;
    
    enhancer2x->Shutdown();
    enhancer4x->Shutdown();
    denoiser->Shutdown();
    delete enhancer2x;
    delete enhancer4x;
    delete denoiser;
    
    printf("\n✓ Performance characteristics demonstrated\n");
}

int main(int argc, char** argv)
{
    printf("\n");
    printf("====================================================\n");
    printf("Graphics Backend + ML Enhancement Integration Demo\n");
    printf("====================================================\n");
    
    Example1_BasicBackend();
    Example2_TextureUpscaling();
    Example3_EnhancementPipeline();
    Example4_PerformanceTest();
    
    printf("\n====================================================\n");
    printf("All examples completed successfully!\n");
    printf("====================================================\n");
    printf("\n");
    
    return 0;
}

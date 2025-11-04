// Standalone test that doesn't require full THUG compilation
// Tests graphics backend and ML system in isolation

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Define basic types for standalone build
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t sint32;

// Include our headers
#include "Backend/GraphicsBackend.h"
#include "ML/GraphicsEnhancer.h"

int main()
{
    printf("\n=== Standalone Graphics System Test ===\n\n");
    
    // Test backend
    printf("Testing Backend Factory:\n");
    Gfx::Backend::IGraphicsBackend* backend = Gfx::Backend::CreateBackend("stub");
    if (backend)
    {
        printf("✓ Backend created: %s\n", backend->GetRendererName());
        
        if (backend->Initialize())
        {
            printf("✓ Backend initialized\n");
            
            // Test some operations
            backend->SetViewport(0, 0, 1920, 1080);
            int x, y, w, h;
            backend->GetViewport(x, y, w, h);
            printf("✓ Viewport set to %dx%d at (%d,%d)\n", w, h, x, y);
            
            backend->Shutdown();
            printf("✓ Backend shut down\n");
        }
        delete backend;
    }
    
    printf("\n");
    
    // Test ML enhancer
    printf("Testing ML Enhancer:\n");
    Gfx::ML::IGraphicsEnhancer* enhancer = Gfx::ML::CreateEnhancer(Gfx::ML::ENHANCEMENT_UPSCALE_2X);
    if (enhancer && enhancer->IsReady())
    {
        printf("✓ Enhancer created: %s v%s\n", 
               enhancer->GetModelName(), 
               enhancer->GetModelVersion());
        
        // Test upscaling
        Gfx::ML::ImageData input;
        input.width = 64;
        input.height = 64;
        input.channels = 4;
        input.data = new float[64 * 64 * 4];
        
        // Fill with test pattern
        for (int i = 0; i < 64 * 64 * 4; i++)
            input.data[i] = 0.5f;
        
        Gfx::ML::ImageData output;
        output.data = nullptr;
        
        if (enhancer->Enhance(input, output))
        {
            printf("✓ Upscaled %dx%d to %dx%d\n", 
                   input.width, input.height,
                   output.width, output.height);
            delete[] output.data;
        }
        
        delete[] input.data;
        enhancer->Shutdown();
        delete enhancer;
    }
    
    printf("\n=== All Tests Passed! ===\n\n");
    return 0;
}

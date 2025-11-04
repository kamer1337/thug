// Simple compilation test for graphics backend and ML system

#include <stdio.h>

// Test that headers can be included
#include "Backend/GraphicsBackend.h"
#include "ML/GraphicsEnhancer.h"

int main()
{
    printf("Graphics Backend and ML Enhancement System\n");
    printf("Testing basic compilation...\n");
    
    // Test backend
    Gfx::Backend::IGraphicsBackend* backend = Gfx::Backend::CreateBackend("stub");
    if (backend)
    {
        printf("✓ Backend factory works\n");
        backend->Initialize();
        printf("✓ Backend initialized\n");
        backend->Shutdown();
        delete backend;
    }
    
    // Test ML enhancer
    Gfx::ML::IGraphicsEnhancer* enhancer = Gfx::ML::CreateEnhancer(Gfx::ML::ENHANCEMENT_UPSCALE_2X);
    if (enhancer)
    {
        printf("✓ ML enhancer factory works\n");
        printf("  Model: %s\n", enhancer->GetModelName());
        enhancer->Shutdown();
        delete enhancer;
    }
    
    printf("\nCompilation test passed!\n");
    return 0;
}

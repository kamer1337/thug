/**
 * Simple integration test to verify Vulkan backend integration compiles
 */

#define USE_VULKAN_RENDERER 1

#include <stdio.h>
#include <gfx/nx.h>

int main()
{
    printf("Testing Vulkan backend integration...\n");
    
    // Test model creation
    Nx::CModel* pModel = Nx::CEngine::sInitModel();
    if (pModel)
    {
        printf("✓ Model created successfully\n");
        Nx::CEngine::sUninitModel(pModel);
    }
    else
    {
        printf("✗ Failed to create model\n");
        return 1;
    }
    
    printf("✓ Integration test passed!\n");
    return 0;
}

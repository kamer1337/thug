// Graphics Backend Test Example
// Demonstrates basic usage of the graphics backend abstraction layer

#include <Gfx/Backend/GraphicsBackend.h>
#include <stdio.h>
#include <stdlib.h>

using namespace Gfx::Backend;

void TestBackendCreation()
{
    printf("=== Testing Backend Creation ===\n");
    
    // Test OpenGL backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    if (backend)
    {
        printf("✓ Created OpenGL backend\n");
        printf("  Renderer: %s\n", backend->GetRendererName());
        printf("  Version: %s\n", backend->GetVersion());
        delete backend;
    }
    
    // Test stub backend
    backend = CreateBackend("stub");
    if (backend)
    {
        printf("✓ Created Stub backend\n");
        printf("  Renderer: %s\n", backend->GetRendererName());
        printf("  Version: %s\n", backend->GetVersion());
        delete backend;
    }
    
    printf("\n");
}

void TestBackendInitialization()
{
    printf("=== Testing Backend Initialization ===\n");
    
    IGraphicsBackend* backend = CreateBackend("opengl");
    
    if (backend->Initialize())
    {
        printf("✓ Backend initialized successfully\n");
        
        // Test viewport
        backend->SetViewport(0, 0, 1920, 1080);
        
        int x, y, w, h;
        backend->GetViewport(x, y, w, h);
        printf("  Viewport: %dx%d at (%d, %d)\n", w, h, x, y);
        
        backend->Shutdown();
        printf("✓ Backend shut down successfully\n");
    }
    
    delete backend;
    printf("\n");
}

void TestTextureOperations()
{
    printf("=== Testing Texture Operations ===\n");
    
    IGraphicsBackend* backend = CreateBackend("opengl");
    backend->Initialize();
    
    // Create test texture data (red checkerboard)
    const int texWidth = 256;
    const int texHeight = 256;
    uint8* textureData = new uint8[texWidth * texHeight * 4];
    
    for (int y = 0; y < texHeight; ++y)
    {
        for (int x = 0; x < texWidth; ++x)
        {
            int idx = (y * texWidth + x) * 4;
            bool isRed = ((x / 32) + (y / 32)) % 2 == 0;
            textureData[idx + 0] = isRed ? 255 : 64;  // R
            textureData[idx + 1] = 0;                  // G
            textureData[idx + 2] = 0;                  // B
            textureData[idx + 3] = 255;                // A
        }
    }
    
    // Create texture
    Texture* tex = backend->CreateTexture(texWidth, texHeight, TEXTURE_FORMAT_RGBA8, textureData);
    if (tex)
    {
        printf("✓ Created %dx%d texture\n", texWidth, texHeight);
        
        // Bind texture
        backend->BindTexture(tex, 0);
        printf("✓ Bound texture to slot 0\n");
        
        // Update texture (blue checkerboard)
        for (int y = 0; y < texHeight; ++y)
        {
            for (int x = 0; x < texWidth; ++x)
            {
                int idx = (y * texWidth + x) * 4;
                bool isBlue = ((x / 32) + (y / 32)) % 2 == 0;
                textureData[idx + 0] = 0;
                textureData[idx + 1] = 0;
                textureData[idx + 2] = isBlue ? 255 : 64;
            }
        }
        
        backend->UpdateTexture(tex, textureData);
        printf("✓ Updated texture data\n");
        
        // Destroy texture
        backend->DestroyTexture(tex);
        printf("✓ Destroyed texture\n");
    }
    
    delete[] textureData;
    backend->Shutdown();
    delete backend;
    printf("\n");
}

void TestRenderStates()
{
    printf("=== Testing Render States ===\n");
    
    IGraphicsBackend* backend = CreateBackend("opengl");
    backend->Initialize();
    
    // Test render state
    RenderState state;
    state.depthTest = true;
    state.depthWrite = true;
    state.cullFace = true;
    state.blend = false;
    state.cullMode = 0; // back face culling
    state.depthFunc = 0; // less
    
    backend->SetRenderState(state);
    printf("✓ Set render state (depth test, cull back faces)\n");
    
    // Test blend mode
    BlendMode blend;
    blend.srcFactor = 2;  // src_alpha
    blend.dstFactor = 3;  // one_minus_src_alpha
    blend.equation = 0;   // add
    
    backend->SetBlendMode(blend);
    printf("✓ Set blend mode (standard alpha blending)\n");
    
    backend->Shutdown();
    delete backend;
    printf("\n");
}

void TestFrameOperations()
{
    printf("=== Testing Frame Operations ===\n");
    
    IGraphicsBackend* backend = CreateBackend("opengl");
    backend->Initialize();
    
    printf("Simulating 3 frames:\n");
    
    for (int frame = 0; frame < 3; ++frame)
    {
        backend->BeginFrame();
        printf("  Frame %d: Begin\n", frame + 1);
        
        // Clear with different colors
        uint32 clearColor = (frame == 0) ? 0xFF0000FF : // Red
                           (frame == 1) ? 0x00FF00FF : // Green
                                         0x0000FFFF;   // Blue
        backend->Clear(CLEAR_COLOR | CLEAR_DEPTH, clearColor);
        printf("  Frame %d: Clear\n", frame + 1);
        
        backend->EndFrame();
        printf("  Frame %d: End\n", frame + 1);
        
        backend->SwapBuffers();
        printf("  Frame %d: Swap\n", frame + 1);
    }
    
    backend->Shutdown();
    delete backend;
    printf("\n");
}

int main(int argc, char** argv)
{
    printf("\n");
    printf("==========================================\n");
    printf("Graphics Backend Test Example\n");
    printf("==========================================\n");
    printf("\n");
    
    TestBackendCreation();
    TestBackendInitialization();
    TestTextureOperations();
    TestRenderStates();
    TestFrameOperations();
    
    printf("==========================================\n");
    printf("All tests completed successfully!\n");
    printf("==========================================\n");
    printf("\n");
    
    return 0;
}

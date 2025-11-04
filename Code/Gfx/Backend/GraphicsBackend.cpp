// Graphics Backend Factory Implementation
// Creates and manages graphics backend instances

#include "GraphicsBackend.h"
#include "OpenGLBackend.h"

#ifdef __THUG_BUILD__
#include <Core/Defines.h>
#else
#include <stdint.h>
#endif

#include <string.h>

namespace Gfx
{
namespace Backend
{

// Global backend instance
static IGraphicsBackend* s_pCurrentBackend = nullptr;

// Stub backend for when no real backend is available
class StubBackend : public IGraphicsBackend
{
public:
    virtual bool Initialize() { return true; }
    virtual void Shutdown() {}
    
    virtual void BeginFrame() {}
    virtual void EndFrame() {}
    virtual void SwapBuffers() {}
    
    virtual void Clear(uint32 flags, uint32 color, float depth, uint32 stencil) {}
    
    virtual void SetViewport(int x, int y, int width, int height) {}
    virtual void GetViewport(int& x, int& y, int& width, int& height) { x = y = 0; width = height = 640; }
    
    virtual void SetRenderState(const RenderState& state) {}
    virtual void SetBlendMode(const BlendMode& mode) {}
    
    virtual Texture* CreateTexture(int width, int height, int format, const void* data) { return nullptr; }
    virtual void DestroyTexture(Texture* texture) {}
    virtual void BindTexture(Texture* texture, int slot) {}
    virtual void UpdateTexture(Texture* texture, const void* data) {}
    
    virtual Mesh* CreateMesh(const void* vertices, int vertexCount, const void* indices, int indexCount) { return nullptr; }
    virtual void DestroyMesh(Mesh* mesh) {}
    virtual void DrawMesh(Mesh* mesh) {}
    
    virtual Shader* CreateShader(const char* vertexSource, const char* fragmentSource) { return nullptr; }
    virtual void DestroyShader(Shader* shader) {}
    virtual void BindShader(Shader* shader) {}
    virtual void SetShaderUniform(Shader* shader, const char* name, const void* data, int type) {}
    
    virtual void SetProjectionMatrix(const Mth::Matrix& matrix) {}
    virtual void SetViewMatrix(const Mth::Matrix& matrix) {}
    virtual void SetModelMatrix(const Mth::Matrix& matrix) {}
    
    virtual const char* GetRendererName() const { return "Stub Backend"; }
    virtual const char* GetVersion() const { return "1.0"; }
    virtual bool SupportsFeature(const char* feature) const { return false; }
};

IGraphicsBackend* CreateBackend(const char* backendType)
{
    if (strcmp(backendType, "opengl") == 0)
    {
        return new OpenGLBackend();
    }
    else if (strcmp(backendType, "stub") == 0)
    {
        return new StubBackend();
    }
    
    // Default to stub backend
    return new StubBackend();
}

IGraphicsBackend* GetBackend()
{
    return s_pCurrentBackend;
}

void SetBackend(IGraphicsBackend* backend)
{
    s_pCurrentBackend = backend;
}

} // namespace Backend
} // namespace Gfx

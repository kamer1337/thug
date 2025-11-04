// Graphics Backend Abstraction Layer
// Provides a unified interface for different rendering backends (OpenGL, DirectX, Vulkan)

#ifndef __GFX_BACKEND_GRAPHICS_BACKEND_H
#define __GFX_BACKEND_GRAPHICS_BACKEND_H

// Include THUG types if available, otherwise use standard types
#ifdef __THUG_BUILD__
#include <Core/Defines.h>
#include <Core/Math/math.h>
#else
#include <stdint.h>
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t sint32;
namespace Mth {
    struct Matrix {
        float m[4][4];
        Matrix() { for(int i=0; i<4; i++) for(int j=0; j<4; j++) m[i][j] = (i==j)?1.0f:0.0f; }
    };
}
#endif

namespace Gfx
{
namespace Backend
{

// Base resource structures (empty, used for type safety)
struct Texture {};
struct Mesh {};
struct Shader {};

// Clear flags
enum ClearFlags
{
    CLEAR_COLOR = 0x01,
    CLEAR_DEPTH = 0x02,
    CLEAR_STENCIL = 0x04
};

// Texture formats
enum TextureFormat
{
    TEXTURE_FORMAT_RGBA8,
    TEXTURE_FORMAT_RGB8,
    TEXTURE_FORMAT_BGRA8,
    TEXTURE_FORMAT_DXT1,
    TEXTURE_FORMAT_DXT3,
    TEXTURE_FORMAT_DXT5,
    TEXTURE_FORMAT_DEPTH24_STENCIL8
};

// Render state structure
struct RenderState
{
    bool depthTest;
    bool depthWrite;
    bool cullFace;
    bool blend;
    int cullMode; // 0=back, 1=front, 2=none
    int depthFunc; // 0=less, 1=lequal, 2=equal, 3=greater, 4=always
    
    RenderState()
        : depthTest(true)
        , depthWrite(true)
        , cullFace(true)
        , blend(false)
        , cullMode(0)
        , depthFunc(0)
    {}
};

// Blend mode structure
struct BlendMode
{
    int srcFactor; // 0=zero, 1=one, 2=src_alpha, 3=one_minus_src_alpha, etc.
    int dstFactor;
    int equation; // 0=add, 1=subtract, 2=reverse_subtract
    
    BlendMode()
        : srcFactor(2)
        , dstFactor(3)
        , equation(0)
    {}
};

// Graphics Backend Interface
class IGraphicsBackend
{
public:
    virtual ~IGraphicsBackend() {}
    
    // Initialization and cleanup
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // Frame operations
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void SwapBuffers() = 0;
    
    // Clear operations
    virtual void Clear(uint32 flags, uint32 color = 0, float depth = 1.0f, uint32 stencil = 0) = 0;
    
    // Viewport management
    virtual void SetViewport(int x, int y, int width, int height) = 0;
    virtual void GetViewport(int& x, int& y, int& width, int& height) = 0;
    
    // Render state management
    virtual void SetRenderState(const RenderState& state) = 0;
    virtual void SetBlendMode(const BlendMode& mode) = 0;
    
    // Texture operations
    virtual Texture* CreateTexture(int width, int height, int format, const void* data) = 0;
    virtual void DestroyTexture(Texture* texture) = 0;
    virtual void BindTexture(Texture* texture, int slot = 0) = 0;
    virtual void UpdateTexture(Texture* texture, const void* data) = 0;
    
    // Mesh operations
    virtual Mesh* CreateMesh(const void* vertices, int vertexCount, const void* indices, int indexCount) = 0;
    virtual void DestroyMesh(Mesh* mesh) = 0;
    virtual void DrawMesh(Mesh* mesh) = 0;
    
    // Shader operations
    virtual Shader* CreateShader(const char* vertexSource, const char* fragmentSource) = 0;
    virtual void DestroyShader(Shader* shader) = 0;
    virtual void BindShader(Shader* shader) = 0;
    virtual void SetShaderUniform(Shader* shader, const char* name, const void* data, int type) = 0;
    
    // Matrix operations
    virtual void SetProjectionMatrix(const Mth::Matrix& matrix) = 0;
    virtual void SetViewMatrix(const Mth::Matrix& matrix) = 0;
    virtual void SetModelMatrix(const Mth::Matrix& matrix) = 0;
    
    // Query capabilities
    virtual const char* GetRendererName() const = 0;
    virtual const char* GetVersion() const = 0;
    virtual bool SupportsFeature(const char* feature) const = 0;
};

// Factory function to create backend based on type
IGraphicsBackend* CreateBackend(const char* backendType);

// Get the current active backend
IGraphicsBackend* GetBackend();

// Set the current active backend
void SetBackend(IGraphicsBackend* backend);

} // namespace Backend
} // namespace Gfx

#endif // __GFX_BACKEND_GRAPHICS_BACKEND_H

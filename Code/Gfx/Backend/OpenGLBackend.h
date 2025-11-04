// OpenGL Graphics Backend Implementation
// Provides OpenGL 3.3+ rendering backend

#ifndef __GFX_BACKEND_OPENGL_BACKEND_H
#define __GFX_BACKEND_OPENGL_BACKEND_H

#include "GraphicsBackend.h"
#include <vector>

namespace Gfx
{
namespace Backend
{

// OpenGL texture structure
struct GLTexture : public Texture
{
    unsigned int id;
    int width;
    int height;
    int format;
};

// OpenGL mesh structure
struct GLMesh : public Mesh
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    int indexCount;
};

// OpenGL shader structure
struct GLShader : public Shader
{
    unsigned int program;
};

// OpenGL Backend implementation
class OpenGLBackend : public IGraphicsBackend
{
public:
    OpenGLBackend();
    virtual ~OpenGLBackend();
    
    // IGraphicsBackend interface
    virtual bool Initialize() override;
    virtual void Shutdown() override;
    
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual void SwapBuffers() override;
    
    virtual void Clear(uint32 flags, uint32 color, float depth, uint32 stencil) override;
    
    virtual void SetViewport(int x, int y, int width, int height) override;
    virtual void GetViewport(int& x, int& y, int& width, int& height) override;
    
    virtual void SetRenderState(const RenderState& state) override;
    virtual void SetBlendMode(const BlendMode& mode) override;
    
    virtual Texture* CreateTexture(int width, int height, int format, const void* data) override;
    virtual void DestroyTexture(Texture* texture) override;
    virtual void BindTexture(Texture* texture, int slot) override;
    virtual void UpdateTexture(Texture* texture, const void* data) override;
    
    virtual Mesh* CreateMesh(const void* vertices, int vertexCount, const void* indices, int indexCount) override;
    virtual void DestroyMesh(Mesh* mesh) override;
    virtual void DrawMesh(Mesh* mesh) override;
    
    virtual Shader* CreateShader(const char* vertexSource, const char* fragmentSource) override;
    virtual void DestroyShader(Shader* shader) override;
    virtual void BindShader(Shader* shader) override;
    virtual void SetShaderUniform(Shader* shader, const char* name, const void* data, int type) override;
    
    virtual void SetProjectionMatrix(const Mth::Matrix& matrix) override;
    virtual void SetViewMatrix(const Mth::Matrix& matrix) override;
    virtual void SetModelMatrix(const Mth::Matrix& matrix) override;
    
    virtual const char* GetRendererName() const override;
    virtual const char* GetVersion() const override;
    virtual bool SupportsFeature(const char* feature) const override;

private:
    bool m_initialized;
    int m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight;
    RenderState m_currentRenderState;
    BlendMode m_currentBlendMode;
    
    Mth::Matrix m_projectionMatrix;
    Mth::Matrix m_viewMatrix;
    Mth::Matrix m_modelMatrix;
    
    std::vector<GLTexture*> m_textures;
    std::vector<GLMesh*> m_meshes;
    std::vector<GLShader*> m_shaders;
    
    // Helper functions
    unsigned int CompileShader(unsigned int type, const char* source);
    unsigned int LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    int GetGLTextureFormat(int format);
    int GetGLBlendFactor(int factor);
    int GetGLBlendEquation(int equation);
};

} // namespace Backend
} // namespace Gfx

#endif // __GFX_BACKEND_OPENGL_BACKEND_H

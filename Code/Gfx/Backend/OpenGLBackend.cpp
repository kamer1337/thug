// OpenGL Graphics Backend Implementation

#include "OpenGLBackend.h"

#ifdef __THUG_BUILD__
#include <Core/Defines.h>
#else
#include <stdint.h>
#endif

#include <stdio.h>
#include <string.h>

// OpenGL includes and definitions
#ifdef USE_OPENGL_RENDERER
    // Use system OpenGL headers when available
    #ifdef _WIN32
        #include <windows.h>
        #include <GL/gl.h>
        #define GL_ELEMENT_ARRAY_BUFFER 0x8893
        #define GL_ARRAY_BUFFER 0x8892
        #define GL_STATIC_DRAW 0x88E4
        #define GL_FRAGMENT_SHADER 0x8B30
        #define GL_VERTEX_SHADER 0x8B31
        #define GL_COMPILE_STATUS 0x8B81
        #define GL_LINK_STATUS 0x8B82
        #define GL_CLAMP_TO_EDGE 0x812F
        #define GL_BGRA_EXT 0x80E1
        #define GL_TEXTURE_WRAP_S 0x2802
        #define GL_TEXTURE_WRAP_T 0x2803
        #define GL_FUNC_ADD 0x8006
        #define GL_FUNC_SUBTRACT 0x800A
        #define GL_FUNC_REVERSE_SUBTRACT 0x800B
        #define GL_NO_ERROR 0
        // Add function pointer types for shader info log
        typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei maxLength, GLsizei *length, char *infoLog);
        typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei maxLength, GLsizei *length, char *infoLog);
        static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
        static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
    #elif defined(__APPLE__)
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>
    #else
        // Linux/Unix
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include <GL/glext.h>
        // Ensure we have required constants
        #ifndef GL_BGRA_EXT
        #define GL_BGRA_EXT 0x80E1
        #endif
        #ifndef GL_CLAMP_TO_EDGE
        #define GL_CLAMP_TO_EDGE 0x812F
        #endif
        #ifndef GL_NO_ERROR
        #define GL_NO_ERROR 0
        #endif
    #endif
    
    // Define function pointers for OpenGL functions that may not be in core GL headers
    #ifdef _WIN32
        // Windows needs function pointers for extended GL functions
        typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
        typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
        typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
        typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, ptrdiff_t size, const void *data, GLenum usage);
        typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
        typedef void (APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const char **string, const GLint *length);
        typedef void (APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
        typedef void (APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
        typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
        typedef void (APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
        typedef void (APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
        typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
        typedef void (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
        typedef void (APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
        typedef void (APIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
        typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const char *name);
        typedef void (APIENTRY *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
        typedef void (APIENTRY *PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
        typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);
        typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
        typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
        typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
        
        // Global function pointers
        static PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
        static PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
        static PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
        static PFNGLBUFFERDATAPROC glBufferData = nullptr;
        static PFNGLCREATESHADERPROC glCreateShader = nullptr;
        static PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
        static PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
        static PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
        static PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
        static PFNGLATTACHSHADERPROC glAttachShader = nullptr;
        static PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
        static PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
        static PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
        static PFNGLDELETESHADERPROC glDeleteShader = nullptr;
        static PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
        static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
        static PFNGLUNIFORM1FPROC glUniform1f = nullptr;
        static PFNGLUNIFORM4FPROC glUniform4f = nullptr;
        static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
        static PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
        static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
        static PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
        static PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
        
        static void LoadGLExtensions()
        {
            glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
            glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
            glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
            glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
            glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
            glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
            glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
            glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
            glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
            glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
            glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
            glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
            glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
            glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
            glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
            glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
            glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
            glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
            glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
            glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
            glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
            glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
            glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
            glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
            glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        }
    #endif
#else
    // Stub OpenGL types when OpenGL is not available
    typedef unsigned int GLuint;
    typedef int GLint;
    typedef unsigned int GLenum;
    typedef float GLfloat;
    typedef unsigned char GLubyte;
    typedef int GLsizei;
    typedef unsigned char GLboolean;
    typedef unsigned int GLbitfield;
    
    #define GL_COLOR_BUFFER_BIT 0x00004000
    #define GL_DEPTH_BUFFER_BIT 0x00000100
    #define GL_STENCIL_BUFFER_BIT 0x00000400
    #define GL_DEPTH_TEST 0x0B71
    #define GL_CULL_FACE 0x0B44
    #define GL_BLEND 0x0BE2
    #define GL_TEXTURE_2D 0x0DE1
    #define GL_RGBA 0x1908
    #define GL_UNSIGNED_BYTE 0x1401
    #define GL_LINEAR 0x2601
    #define GL_TEXTURE_MIN_FILTER 0x2801
    #define GL_TEXTURE_MAG_FILTER 0x2800
    #define GL_TEXTURE0 0x84C0
    #define GL_TRIANGLES 0x0004
#endif

namespace Gfx
{
namespace Backend
{

OpenGLBackend::OpenGLBackend()
    : m_initialized(false)
    , m_viewportX(0)
    , m_viewportY(0)
    , m_viewportWidth(800)
    , m_viewportHeight(600)
{
}

OpenGLBackend::~OpenGLBackend()
{
    Shutdown();
}

bool OpenGLBackend::Initialize()
{
    if (m_initialized)
        return true;
    
#ifdef USE_OPENGL_RENDERER
    printf("OpenGL Backend: Initializing with OpenGL support\n");
    
    #ifdef _WIN32
    // Load OpenGL extension functions on Windows
    LoadGLExtensions();
    #endif
    
    // Query OpenGL version and renderer info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    if (renderer && version)
    {
        printf("  Renderer: %s\n", renderer);
        printf("  Version: %s\n", version);
    }
    
    // Set up default render states
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
#else
    printf("OpenGL Backend: Initializing (stub implementation - OpenGL not available)\n");
#endif
    
    m_initialized = true;
    return true;
}

void OpenGLBackend::Shutdown()
{
    if (!m_initialized)
        return;
    
    // Clean up all resources
    for (auto texture : m_textures)
        delete texture;
    m_textures.clear();
    
    for (auto mesh : m_meshes)
        delete mesh;
    m_meshes.clear();
    
    for (auto shader : m_shaders)
        delete shader;
    m_shaders.clear();
    
    m_initialized = false;
}

void OpenGLBackend::BeginFrame()
{
#ifdef USE_OPENGL_RENDERER
    // Begin frame timing, clear error state
    glGetError(); // Clear any previous errors
#endif
}

void OpenGLBackend::EndFrame()
{
#ifdef USE_OPENGL_RENDERER
    // End frame timing, check for errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL Error in frame: 0x%x\n", err);
    }
#endif
}

void OpenGLBackend::SwapBuffers()
{
#ifdef USE_OPENGL_RENDERER
    // Note: In a real implementation, this would call platform-specific swap
    // (e.g., glfwSwapBuffers, SDL_GL_SwapWindow, wglSwapBuffers)
    // For now, we leave this as a no-op since we don't manage the window/context
#endif
}

void OpenGLBackend::Clear(uint32 flags, uint32 color, float depth, uint32 stencil)
{
#ifdef USE_OPENGL_RENDERER
    GLbitfield clearFlags = 0;
    
    if (flags & CLEAR_COLOR)
    {
        clearFlags |= GL_COLOR_BUFFER_BIT;
        // Extract RGBA from color (assuming RGBA format)
        float r = ((color >> 24) & 0xFF) / 255.0f;
        float g = ((color >> 16) & 0xFF) / 255.0f;
        float b = ((color >> 8) & 0xFF) / 255.0f;
        float a = (color & 0xFF) / 255.0f;
        glClearColor(r, g, b, a);
    }
    
    if (flags & CLEAR_DEPTH)
    {
        clearFlags |= GL_DEPTH_BUFFER_BIT;
        glClearDepth(depth);
    }
    
    if (flags & CLEAR_STENCIL)
    {
        clearFlags |= GL_STENCIL_BUFFER_BIT;
        glClearStencil(stencil);
    }
    
    if (clearFlags != 0)
    {
        glClear(clearFlags);
    }
#endif
}

void OpenGLBackend::SetViewport(int x, int y, int width, int height)
{
    m_viewportX = x;
    m_viewportY = y;
    m_viewportWidth = width;
    m_viewportHeight = height;
    
#ifdef USE_OPENGL_RENDERER
    glViewport(x, y, width, height);
#endif
}

void OpenGLBackend::GetViewport(int& x, int& y, int& width, int& height)
{
    x = m_viewportX;
    y = m_viewportY;
    width = m_viewportWidth;
    height = m_viewportHeight;
}

void OpenGLBackend::SetRenderState(const RenderState& state)
{
    m_currentRenderState = state;
    
#ifdef USE_OPENGL_RENDERER
    // Depth test
    if (state.depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    // Depth write
    glDepthMask(state.depthWrite ? GL_TRUE : GL_FALSE);
    
    // Cull face
    if (state.cullFace)
    {
        glEnable(GL_CULL_FACE);
        // cullMode: 0=back, 1=front, 2=none
        if (state.cullMode == 0)
            glCullFace(GL_BACK);
        else if (state.cullMode == 1)
            glCullFace(GL_FRONT);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    
    // Blend
    if (state.blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    
    // Depth function: 0=less, 1=lequal, 2=equal, 3=greater, 4=always
    GLenum depthFuncs[] = { GL_LESS, GL_LEQUAL, GL_EQUAL, GL_GREATER, GL_ALWAYS };
    if (state.depthFunc >= 0 && state.depthFunc < 5)
        glDepthFunc(depthFuncs[state.depthFunc]);
#endif
}

void OpenGLBackend::SetBlendMode(const BlendMode& mode)
{
    m_currentBlendMode = mode;
    
#ifdef USE_OPENGL_RENDERER
    // Map blend factors: 0=zero, 1=one, 2=src_alpha, 3=one_minus_src_alpha, etc.
    GLenum blendFactors[] = {
        GL_ZERO,                // 0
        GL_ONE,                 // 1
        GL_SRC_ALPHA,           // 2
        GL_ONE_MINUS_SRC_ALPHA, // 3
        GL_DST_ALPHA,           // 4
        GL_ONE_MINUS_DST_ALPHA, // 5
        GL_SRC_COLOR,           // 6
        GL_ONE_MINUS_SRC_COLOR, // 7
        GL_DST_COLOR,           // 8
        GL_ONE_MINUS_DST_COLOR  // 9
    };
    
    GLenum srcFactor = (mode.srcFactor >= 0 && mode.srcFactor < 10) ? 
                       blendFactors[mode.srcFactor] : GL_SRC_ALPHA;
    GLenum dstFactor = (mode.dstFactor >= 0 && mode.dstFactor < 10) ? 
                       blendFactors[mode.dstFactor] : GL_ONE_MINUS_SRC_ALPHA;
    
    glBlendFunc(srcFactor, dstFactor);
    
    // Blend equation: 0=add, 1=subtract, 2=reverse_subtract
    GLenum equations[] = { GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT };
    if (mode.equation >= 0 && mode.equation < 3)
        glBlendEquation(equations[mode.equation]);
#endif
}

Texture* OpenGLBackend::CreateTexture(int width, int height, int format, const void* data)
{
    GLTexture* texture = new GLTexture();
    texture->width = width;
    texture->height = height;
    texture->format = format;
    texture->id = 0;
    
#ifdef USE_OPENGL_RENDERER
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    
    // Map our format to OpenGL format
    GLenum glFormat = GL_RGBA;
    GLenum glInternalFormat = GL_RGBA;
    
    switch (format)
    {
        case TEXTURE_FORMAT_RGBA8:
            glFormat = GL_RGBA;
            glInternalFormat = GL_RGBA;
            break;
        case TEXTURE_FORMAT_RGB8:
            glFormat = GL_RGB;
            glInternalFormat = GL_RGB;
            break;
        case TEXTURE_FORMAT_BGRA8:
            glFormat = GL_BGRA_EXT;
            glInternalFormat = GL_RGBA;
            break;
        default:
            glFormat = GL_RGBA;
            glInternalFormat = GL_RGBA;
            break;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, 
                 glFormat, GL_UNSIGNED_BYTE, data);
    
    // Set default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
    
    m_textures.push_back(texture);
    return texture;
}

void OpenGLBackend::DestroyTexture(Texture* texture)
{
    if (!texture)
        return;
    
    GLTexture* glTexture = static_cast<GLTexture*>(texture);
    
#ifdef USE_OPENGL_RENDERER
    if (glTexture->id != 0)
    {
        glDeleteTextures(1, &glTexture->id);
    }
#endif
    
    // Remove from vector
    for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
    {
        if (*it == glTexture)
        {
            m_textures.erase(it);
            break;
        }
    }
    
    delete glTexture;
}

void OpenGLBackend::BindTexture(Texture* texture, int slot)
{
    if (!texture)
        return;
    
    GLTexture* glTexture = static_cast<GLTexture*>(texture);
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glActiveTexture)
        glActiveTexture(GL_TEXTURE0 + slot);
    #else
    glActiveTexture(GL_TEXTURE0 + slot);
    #endif
    
    glBindTexture(GL_TEXTURE_2D, glTexture->id);
#endif
}

void OpenGLBackend::UpdateTexture(Texture* texture, const void* data)
{
    if (!texture || !data)
        return;
    
    GLTexture* glTexture = static_cast<GLTexture*>(texture);
    
#ifdef USE_OPENGL_RENDERER
    glBindTexture(GL_TEXTURE_2D, glTexture->id);
    
    GLenum glFormat = GL_RGBA;
    switch (glTexture->format)
    {
        case TEXTURE_FORMAT_RGBA8:
            glFormat = GL_RGBA;
            break;
        case TEXTURE_FORMAT_RGB8:
            glFormat = GL_RGB;
            break;
        case TEXTURE_FORMAT_BGRA8:
            glFormat = GL_BGRA_EXT;
            break;
        default:
            glFormat = GL_RGBA;
            break;
    }
    
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, glTexture->width, glTexture->height,
                    glFormat, GL_UNSIGNED_BYTE, data);
#endif
}

Mesh* OpenGLBackend::CreateMesh(const void* vertices, int vertexCount, const void* indices, int indexCount)
{
    GLMesh* mesh = new GLMesh();
    mesh->vao = 0;
    mesh->vbo = 0;
    mesh->ebo = 0;
    mesh->indexCount = indexCount;
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    // On Windows, use function pointers if available
    if (glGenVertexArrays && glGenBuffers && glBindVertexArray && glBindBuffer && glBufferData)
    {
        glGenVertexArrays(1, &mesh->vao);
        glGenBuffers(1, &mesh->vbo);
        glGenBuffers(1, &mesh->ebo);
        
        glBindVertexArray(mesh->vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 8, vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);
        
        // Note: Vertex attribute setup would go here in a full implementation
        // For now, we just create the buffers
        
        glBindVertexArray(0);
    }
    #else
    // On other platforms, assume functions are available
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);
    
    glBindVertexArray(mesh->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 8, vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    #endif
#endif
    
    m_meshes.push_back(mesh);
    return mesh;
}

void OpenGLBackend::DestroyMesh(Mesh* mesh)
{
    if (!mesh)
        return;
    
    GLMesh* glMesh = static_cast<GLMesh*>(mesh);
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glDeleteVertexArrays && glDeleteBuffers)
    {
        if (glMesh->vao != 0)
            glDeleteVertexArrays(1, &glMesh->vao);
        if (glMesh->vbo != 0)
            glDeleteBuffers(1, &glMesh->vbo);
        if (glMesh->ebo != 0)
            glDeleteBuffers(1, &glMesh->ebo);
    }
    #else
    if (glMesh->vao != 0)
        glDeleteVertexArrays(1, &glMesh->vao);
    if (glMesh->vbo != 0)
        glDeleteBuffers(1, &glMesh->vbo);
    if (glMesh->ebo != 0)
        glDeleteBuffers(1, &glMesh->ebo);
    #endif
#endif
    
    // Remove from vector
    for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
    {
        if (*it == glMesh)
        {
            m_meshes.erase(it);
            break;
        }
    }
    
    delete glMesh;
}

void OpenGLBackend::DrawMesh(Mesh* mesh)
{
    if (!mesh)
        return;
    
    GLMesh* glMesh = static_cast<GLMesh*>(mesh);
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glBindVertexArray && glMesh->vao != 0)
    {
        glBindVertexArray(glMesh->vao);
        glDrawElements(GL_TRIANGLES, glMesh->indexCount, GL_UNSIGNED_SHORT, 0);
    }
    #else
    if (glMesh->vao != 0)
    {
        glBindVertexArray(glMesh->vao);
        glDrawElements(GL_TRIANGLES, glMesh->indexCount, GL_UNSIGNED_SHORT, 0);
    }
    #endif
#endif
}

Shader* OpenGLBackend::CreateShader(const char* vertexSource, const char* fragmentSource)
{
    if (!vertexSource || !fragmentSource)
        return nullptr;
    
    GLShader* shader = new GLShader();
    shader->program = 0;
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glCreateShader && glShaderSource && glCompileShader && glGetShaderiv &&
        glCreateProgram && glAttachShader && glLinkProgram && glGetProgramiv &&
        glDeleteShader)
    {
    #endif
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
        
        if (vs != 0 && fs != 0)
        {
            shader->program = LinkProgram(vs, fs);
        }
        
        #ifdef _WIN32
        if (glDeleteShader)
        {
        #endif
            if (vs != 0) glDeleteShader(vs);
            if (fs != 0) glDeleteShader(fs);
        #ifdef _WIN32
        }
        #endif
    #ifdef _WIN32
    }
    #endif
#endif
    
    m_shaders.push_back(shader);
    return shader;
}

void OpenGLBackend::DestroyShader(Shader* shader)
{
    if (!shader)
        return;
    
    GLShader* glShader = static_cast<GLShader*>(shader);
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glDeleteProgram && glShader->program != 0)
    {
        glDeleteProgram(glShader->program);
    }
    #else
    if (glShader->program != 0)
    {
        glDeleteProgram(glShader->program);
    }
    #endif
#endif
    
    // Remove from vector
    for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
    {
        if (*it == glShader)
        {
            m_shaders.erase(it);
            break;
        }
    }
    
    delete glShader;
}

void OpenGLBackend::BindShader(Shader* shader)
{
    if (!shader)
        return;
    
    GLShader* glShader = static_cast<GLShader*>(shader);
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glUseProgram)
    {
        glUseProgram(glShader->program);
    }
    #else
    glUseProgram(glShader->program);
    #endif
#endif
}

void OpenGLBackend::SetShaderUniform(Shader* shader, const char* name, const void* data, int type)
{
    if (!shader || !name || !data)
        return;
    
    GLShader* glShader = static_cast<GLShader*>(shader);
    
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (glGetUniformLocation && glShader->program != 0)
    {
        GLint location = glGetUniformLocation(glShader->program, name);
        if (location >= 0)
        {
            // type: 0=float, 1=vec4, 2=mat4, etc.
            switch (type)
            {
                case 0: // float
                    if (glUniform1f)
                        glUniform1f(location, *static_cast<const float*>(data));
                    break;
                case 1: // vec4
                    if (glUniform4f)
                    {
                        const float* v = static_cast<const float*>(data);
                        glUniform4f(location, v[0], v[1], v[2], v[3]);
                    }
                    break;
                case 2: // mat4
                    if (glUniformMatrix4fv)
                        glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                    break;
            }
        }
    }
    #else
    GLint location = glGetUniformLocation(glShader->program, name);
    if (location >= 0)
    {
        switch (type)
        {
            case 0: // float
                glUniform1f(location, *static_cast<const float*>(data));
                break;
            case 1: // vec4
            {
                const float* v = static_cast<const float*>(data);
                glUniform4f(location, v[0], v[1], v[2], v[3]);
                break;
            }
            case 2: // mat4
                glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
        }
    }
    #endif
#endif
}

void OpenGLBackend::SetProjectionMatrix(const Mth::Matrix& matrix)
{
    m_projectionMatrix = matrix;
}

void OpenGLBackend::SetViewMatrix(const Mth::Matrix& matrix)
{
    m_viewMatrix = matrix;
}

void OpenGLBackend::SetModelMatrix(const Mth::Matrix& matrix)
{
    m_modelMatrix = matrix;
}

const char* OpenGLBackend::GetRendererName() const
{
#ifdef USE_OPENGL_RENDERER
    return "OpenGL Backend";
#else
    return "OpenGL Backend (Stub - OpenGL not available)";
#endif
}

const char* OpenGLBackend::GetVersion() const
{
#ifdef USE_OPENGL_RENDERER
    const GLubyte* version = glGetString(GL_VERSION);
    return version ? reinterpret_cast<const char*>(version) : "Unknown";
#else
    return "3.3 (Stub)";
#endif
}

bool OpenGLBackend::SupportsFeature(const char* feature) const
{
#ifdef USE_OPENGL_RENDERER
    // In a real implementation, check for specific OpenGL extensions
    // For now, return false
    return false;
#else
    return false;
#endif
}

unsigned int OpenGLBackend::CompileShader(unsigned int type, const char* source)
{
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (!glCreateShader || !glShaderSource || !glCompileShader || !glGetShaderiv)
        return 0;
    #endif
    
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        #ifdef _WIN32
        if (glGetShaderInfoLog)
        {
        #endif
            char infoLog[512];
            GLsizei length;
            glGetShaderInfoLog(shader, 512, &length, infoLog);
            printf("Shader compilation failed: %s\n", infoLog);
        #ifdef _WIN32
        }
        else
        {
            printf("Shader compilation failed (no error details available)\n");
        }
        #endif
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
#else
    return 0;
#endif
}

unsigned int OpenGLBackend::LinkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
#ifdef USE_OPENGL_RENDERER
    #ifdef _WIN32
    if (!glCreateProgram || !glAttachShader || !glLinkProgram || !glGetProgramiv)
        return 0;
    #endif
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        #ifdef _WIN32
        if (glGetProgramInfoLog)
        {
        #endif
            char infoLog[512];
            GLsizei length;
            glGetProgramInfoLog(program, 512, &length, infoLog);
            printf("Shader program linking failed: %s\n", infoLog);
        #ifdef _WIN32
        }
        else
        {
            printf("Shader program linking failed (no error details available)\n");
        }
        #endif
        glDeleteProgram(program);
        return 0;
    }
    
    return program;
#else
    return 0;
#endif
}

int OpenGLBackend::GetGLTextureFormat(int format)
{
#ifdef USE_OPENGL_RENDERER
    switch (format)
    {
        case TEXTURE_FORMAT_RGBA8:
            return GL_RGBA;
        case TEXTURE_FORMAT_RGB8:
            return GL_RGB;
        case TEXTURE_FORMAT_BGRA8:
            return GL_BGRA_EXT;
        default:
            return GL_RGBA;
    }
#else
    return 0;
#endif
}

int OpenGLBackend::GetGLBlendFactor(int factor)
{
#ifdef USE_OPENGL_RENDERER
    switch (factor)
    {
        case 0: return GL_ZERO;
        case 1: return GL_ONE;
        case 2: return GL_SRC_ALPHA;
        case 3: return GL_ONE_MINUS_SRC_ALPHA;
        case 4: return GL_DST_ALPHA;
        case 5: return GL_ONE_MINUS_DST_ALPHA;
        case 6: return GL_SRC_COLOR;
        case 7: return GL_ONE_MINUS_SRC_COLOR;
        case 8: return GL_DST_COLOR;
        case 9: return GL_ONE_MINUS_DST_COLOR;
        default: return GL_SRC_ALPHA;
    }
#else
    return 0;
#endif
}

int OpenGLBackend::GetGLBlendEquation(int equation)
{
#ifdef USE_OPENGL_RENDERER
    switch (equation)
    {
        case 0: return GL_FUNC_ADD;
        case 1: return GL_FUNC_SUBTRACT;
        case 2: return GL_FUNC_REVERSE_SUBTRACT;
        default: return GL_FUNC_ADD;
    }
#else
    return 0;
#endif
}

} // namespace Backend
} // namespace Gfx

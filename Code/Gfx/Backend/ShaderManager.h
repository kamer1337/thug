// Shader Manager - Manages shader creation, compilation, and caching
// Provides easy access to predefined shaders and custom shader loading

#ifndef __GFX_BACKEND_SHADER_MANAGER_H
#define __GFX_BACKEND_SHADER_MANAGER_H

#include "GraphicsBackend.h"
#include "ShaderLibrary.h"
#include <map>
#include <string>

namespace Gfx
{
namespace Backend
{

// Shader types for easy access to predefined shaders
enum ShaderType
{
    SHADER_TYPE_BASIC,
    SHADER_TYPE_PHONG,
    SHADER_TYPE_PBR,
    SHADER_TYPE_SHADOW_MAP,
    SHADER_TYPE_SHADOW_MAPPING,
    SHADER_TYPE_SCREEN_QUAD,
    SHADER_TYPE_BLOOM,
    SHADER_TYPE_BLUR,
    SHADER_TYPE_SSAO,
    SHADER_TYPE_CUSTOM
};

// Shader program wrapper with metadata
struct ShaderProgram
{
    Shader* shader;
    ShaderType type;
    std::string name;
    bool isValid;
    
    ShaderProgram()
        : shader(nullptr)
        , type(SHADER_TYPE_CUSTOM)
        , name("")
        , isValid(false)
    {}
};

// Shader Manager class
class ShaderManager
{
public:
    ShaderManager(IGraphicsBackend* backend);
    ~ShaderManager();
    
    // Initialize with default shaders
    bool Initialize();
    
    // Shutdown and cleanup
    void Shutdown();
    
    // Get predefined shader by type
    ShaderProgram* GetShader(ShaderType type);
    
    // Get shader by name
    ShaderProgram* GetShader(const char* name);
    
    // Create custom shader from source
    ShaderProgram* CreateShader(const char* name, const char* vertexSource, const char* fragmentSource);
    
    // Load shader from files
    ShaderProgram* LoadShader(const char* name, const char* vertexFile, const char* fragmentFile);
    
    // Check if shader exists
    bool HasShader(const char* name) const;
    
    // Delete shader
    void DeleteShader(const char* name);
    
    // Get all shader names
    void GetShaderNames(std::vector<std::string>& names) const;
    
    // Bind shader (convenience function)
    void BindShader(const char* name);
    void BindShader(ShaderType type);
    
    // Set uniform helpers (uses currently bound shader)
    void SetUniformFloat(const char* name, float value);
    void SetUniformVec3(const char* name, float x, float y, float z);
    void SetUniformVec4(const char* name, float x, float y, float z, float w);
    void SetUniformMat4(const char* name, const float* matrix);
    void SetUniformInt(const char* name, int value);
    void SetUniformBool(const char* name, bool value);
    
private:
    IGraphicsBackend* m_backend;
    std::map<std::string, ShaderProgram> m_shaders;
    std::map<ShaderType, std::string> m_typeToName;
    ShaderProgram* m_currentShader;
    
    // Helper to create predefined shaders
    bool CreatePredefinedShader(const char* name, ShaderType type, const char* vs, const char* fs);
    
    // Helper to read file contents
    bool ReadFile(const char* filename, std::string& content);
};

} // namespace Backend
} // namespace Gfx

#endif // __GFX_BACKEND_SHADER_MANAGER_H

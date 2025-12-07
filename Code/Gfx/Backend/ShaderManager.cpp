// Shader Manager Implementation

#include "ShaderManager.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

namespace Gfx
{
namespace Backend
{

ShaderManager::ShaderManager(IGraphicsBackend* backend)
    : m_backend(backend)
    , m_currentShader(nullptr)
{
}

ShaderManager::~ShaderManager()
{
    Shutdown();
}

bool ShaderManager::Initialize()
{
    if (!m_backend)
    {
        printf("ShaderManager: No backend provided\n");
        return false;
    }
    
    printf("ShaderManager: Initializing with predefined shaders...\n");
    
    // Create predefined shaders
    bool success = true;
    
    success &= CreatePredefinedShader("basic", SHADER_TYPE_BASIC,
                                      SHADER_BASIC_VERTEX, SHADER_BASIC_FRAGMENT);
    
    success &= CreatePredefinedShader("phong", SHADER_TYPE_PHONG,
                                      SHADER_PHONG_VERTEX, SHADER_PHONG_FRAGMENT);
    
    success &= CreatePredefinedShader("pbr", SHADER_TYPE_PBR,
                                      SHADER_PBR_VERTEX, SHADER_PBR_FRAGMENT);
    
    success &= CreatePredefinedShader("shadow_map", SHADER_TYPE_SHADOW_MAP,
                                      SHADER_SHADOW_VERTEX, SHADER_SHADOW_FRAGMENT);
    
    success &= CreatePredefinedShader("shadow_mapping", SHADER_TYPE_SHADOW_MAPPING,
                                      SHADER_SHADOW_MAPPING_VERTEX, SHADER_SHADOW_MAPPING_FRAGMENT);
    
    success &= CreatePredefinedShader("bloom", SHADER_TYPE_BLOOM,
                                      SHADER_SCREEN_QUAD_VERTEX, SHADER_BLOOM_FRAGMENT);
    
    success &= CreatePredefinedShader("blur", SHADER_TYPE_BLUR,
                                      SHADER_SCREEN_QUAD_VERTEX, SHADER_BLUR_FRAGMENT);
    
    success &= CreatePredefinedShader("ssao", SHADER_TYPE_SSAO,
                                      SHADER_SCREEN_QUAD_VERTEX, SHADER_SSAO_FRAGMENT);
    
    printf("ShaderManager: Initialized with %lu shaders\n", 
           (unsigned long)m_shaders.size());
    
    return success;
}

void ShaderManager::Shutdown()
{
    printf("ShaderManager: Shutting down, cleaning up %lu shaders\n",
           (unsigned long)m_shaders.size());
    
    // Delete all shaders
    for (auto& pair : m_shaders)
    {
        if (pair.second.shader && m_backend)
        {
            m_backend->DestroyShader(pair.second.shader);
        }
    }
    
    m_shaders.clear();
    m_typeToName.clear();
    m_currentShader = nullptr;
}

ShaderProgram* ShaderManager::GetShader(ShaderType type)
{
    auto it = m_typeToName.find(type);
    if (it != m_typeToName.end())
    {
        return GetShader(it->second.c_str());
    }
    return nullptr;
}

ShaderProgram* ShaderManager::GetShader(const char* name)
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end())
    {
        return &it->second;
    }
    return nullptr;
}

ShaderProgram* ShaderManager::CreateShader(const char* name, 
                                          const char* vertexSource, 
                                          const char* fragmentSource)
{
    if (!name || !vertexSource || !fragmentSource)
    {
        printf("ShaderManager: Invalid parameters for CreateShader\n");
        return nullptr;
    }
    
    // Check if shader already exists
    if (HasShader(name))
    {
        printf("ShaderManager: Shader '%s' already exists\n", name);
        return GetShader(name);
    }
    
    // Create shader
    Shader* shader = m_backend->CreateShader(vertexSource, fragmentSource);
    if (!shader)
    {
        printf("ShaderManager: Failed to create shader '%s'\n", name);
        return nullptr;
    }
    
    // Add to map
    ShaderProgram program;
    program.shader = shader;
    program.type = SHADER_TYPE_CUSTOM;
    program.name = name;
    program.isValid = true;
    
    m_shaders[name] = program;
    
    printf("ShaderManager: Created custom shader '%s'\n", name);
    return &m_shaders[name];
}

ShaderProgram* ShaderManager::LoadShader(const char* name,
                                        const char* vertexFile,
                                        const char* fragmentFile)
{
    if (!name || !vertexFile || !fragmentFile)
    {
        printf("ShaderManager: Invalid parameters for LoadShader\n");
        return nullptr;
    }
    
    // Read vertex shader
    std::string vertexSource;
    if (!ReadFile(vertexFile, vertexSource))
    {
        printf("ShaderManager: Failed to read vertex shader file: %s\n", vertexFile);
        return nullptr;
    }
    
    // Read fragment shader
    std::string fragmentSource;
    if (!ReadFile(fragmentFile, fragmentSource))
    {
        printf("ShaderManager: Failed to read fragment shader file: %s\n", fragmentFile);
        return nullptr;
    }
    
    return CreateShader(name, vertexSource.c_str(), fragmentSource.c_str());
}

bool ShaderManager::HasShader(const char* name) const
{
    return m_shaders.find(name) != m_shaders.end();
}

void ShaderManager::DeleteShader(const char* name)
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end())
    {
        if (it->second.shader && m_backend)
        {
            m_backend->DestroyShader(it->second.shader);
        }
        
        // Remove from type map if it's a predefined shader
        for (auto typeIt = m_typeToName.begin(); typeIt != m_typeToName.end(); ++typeIt)
        {
            if (typeIt->second == name)
            {
                m_typeToName.erase(typeIt);
                break;
            }
        }
        
        m_shaders.erase(it);
        printf("ShaderManager: Deleted shader '%s'\n", name);
    }
}

void ShaderManager::GetShaderNames(std::vector<std::string>& names) const
{
    names.clear();
    for (const auto& pair : m_shaders)
    {
        names.push_back(pair.first);
    }
}

void ShaderManager::BindShader(const char* name)
{
    ShaderProgram* program = GetShader(name);
    if (program && program->isValid && program->shader)
    {
        m_backend->BindShader(program->shader);
        m_currentShader = program;
    }
    else
    {
        printf("ShaderManager: Failed to bind shader '%s'\n", name);
    }
}

void ShaderManager::BindShader(ShaderType type)
{
    ShaderProgram* program = GetShader(type);
    if (program && program->isValid && program->shader)
    {
        m_backend->BindShader(program->shader);
        m_currentShader = program;
    }
    else
    {
        printf("ShaderManager: Failed to bind shader type %d\n", type);
    }
}

void ShaderManager::SetUniformFloat(const char* name, float value)
{
    if (m_currentShader && m_currentShader->shader)
    {
        m_backend->SetShaderUniform(m_currentShader->shader, name, &value, 0);
    }
}

void ShaderManager::SetUniformVec3(const char* name, float x, float y, float z)
{
    if (m_currentShader && m_currentShader->shader)
    {
        float values[3] = { x, y, z };
        m_backend->SetShaderUniform(m_currentShader->shader, name, values, 1);
    }
}

void ShaderManager::SetUniformVec4(const char* name, float x, float y, float z, float w)
{
    if (m_currentShader && m_currentShader->shader)
    {
        float values[4] = { x, y, z, w };
        m_backend->SetShaderUniform(m_currentShader->shader, name, values, 1);
    }
}

void ShaderManager::SetUniformMat4(const char* name, const float* matrix)
{
    if (m_currentShader && m_currentShader->shader && matrix)
    {
        m_backend->SetShaderUniform(m_currentShader->shader, name, matrix, 2);
    }
}

void ShaderManager::SetUniformInt(const char* name, int value)
{
    if (m_currentShader && m_currentShader->shader)
    {
        m_backend->SetShaderUniform(m_currentShader->shader, name, &value, 0);
    }
}

void ShaderManager::SetUniformBool(const char* name, bool value)
{
    if (m_currentShader && m_currentShader->shader)
    {
        int intValue = value ? 1 : 0;
        m_backend->SetShaderUniform(m_currentShader->shader, name, &intValue, 0);
    }
}

bool ShaderManager::CreatePredefinedShader(const char* name, ShaderType type,
                                           const char* vs, const char* fs)
{
    ShaderProgram* program = CreateShader(name, vs, fs);
    if (program)
    {
        program->type = type;
        m_typeToName[type] = name;
        return true;
    }
    return false;
}

bool ShaderManager::ReadFile(const char* filename, std::string& content)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();
    
    return true;
}

} // namespace Backend
} // namespace Gfx

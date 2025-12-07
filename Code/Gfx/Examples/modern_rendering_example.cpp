// Modern Rendering Example
// Demonstrates usage of ShaderManager and ModernRenderer

#include "../Backend/GraphicsBackend.h"
#include "../Backend/ShaderManager.h"
#include "../Backend/ModernRenderer.h"
#include <stdio.h>
#include <math.h>

using namespace Gfx::Backend;

// Example: Basic shader usage
void ExampleBasicShaders()
{
    printf("=== Example: Basic Shader Usage ===\n\n");
    
    // Create backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    if (!backend || !backend->Initialize())
    {
        printf("Failed to initialize backend\n");
        delete backend;
        return;
    }
    
    // Create shader manager
    ShaderManager shaderManager(backend);
    if (!shaderManager.Initialize())
    {
        printf("Failed to initialize shader manager\n");
        backend->Shutdown();
        delete backend;
        return;
    }
    
    // List all available shaders
    std::vector<std::string> shaderNames;
    shaderManager.GetShaderNames(shaderNames);
    
    printf("Available shaders (%zu):\n", shaderNames.size());
    for (const auto& name : shaderNames)
    {
        ShaderProgram* program = shaderManager.GetShader(name.c_str());
        if (program)
        {
            printf("  - %s (type: %d, valid: %s)\n", 
                   name.c_str(), 
                   program->type,
                   program->isValid ? "yes" : "no");
        }
    }
    printf("\n");
    
    // Use basic shader
    printf("Using basic shader:\n");
    shaderManager.BindShader(SHADER_TYPE_BASIC);
    
    // Set some uniforms
    shaderManager.SetUniformVec4("uColor", 1.0f, 0.5f, 0.2f, 1.0f);
    shaderManager.SetUniformBool("uUseTexture", false);
    printf("  Set color uniform\n");
    printf("  Set texture flag\n\n");
    
    // Use Phong shader
    printf("Using Phong lighting shader:\n");
    shaderManager.BindShader(SHADER_TYPE_PHONG);
    
    shaderManager.SetUniformVec3("uLightPos", 5.0f, 10.0f, 5.0f);
    shaderManager.SetUniformVec3("uLightColor", 1.0f, 1.0f, 1.0f);
    shaderManager.SetUniformFloat("uLightIntensity", 1.5f);
    printf("  Set light position and color\n");
    printf("  Set light intensity\n\n");
    
    // Cleanup
    shaderManager.Shutdown();
    backend->Shutdown();
    delete backend;
    
    printf("Example complete!\n\n");
}

// Example: PBR rendering
void ExamplePBRRendering()
{
    printf("=== Example: PBR Rendering ===\n\n");
    
    // Create backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    if (!backend || !backend->Initialize())
    {
        printf("Failed to initialize backend\n");
        delete backend;
        return;
    }
    
    // Create shader manager
    ShaderManager shaderManager(backend);
    shaderManager.Initialize();
    
    // Use PBR shader
    printf("Using PBR shader:\n");
    shaderManager.BindShader(SHADER_TYPE_PBR);
    
    // Set material properties
    printf("Setting material properties:\n");
    shaderManager.SetUniformVec3("uAlbedo", 0.8f, 0.2f, 0.2f);
    shaderManager.SetUniformFloat("uMetallic", 0.3f);
    shaderManager.SetUniformFloat("uRoughness", 0.7f);
    shaderManager.SetUniformFloat("uAO", 1.0f);
    printf("  Albedo: red-ish\n");
    printf("  Metallic: 0.3\n");
    printf("  Roughness: 0.7\n");
    printf("  AO: 1.0\n\n");
    
    // Set multiple lights
    printf("Setting up lights:\n");
    float lightPos1[3] = {10.0f, 10.0f, 10.0f};
    float lightPos2[3] = {-10.0f, 10.0f, 10.0f};
    float lightColor1[3] = {300.0f, 300.0f, 300.0f};
    float lightColor2[3] = {200.0f, 100.0f, 100.0f};
    
    // Note: In real usage, you'd set array uniforms properly
    printf("  Light 1: White light at (10, 10, 10)\n");
    printf("  Light 2: Reddish light at (-10, 10, 10)\n\n");
    
    // Cleanup
    shaderManager.Shutdown();
    backend->Shutdown();
    delete backend;
    
    printf("Example complete!\n\n");
}

// Example: Deferred rendering with modern renderer
void ExampleDeferredRendering()
{
    printf("=== Example: Deferred Rendering ===\n\n");
    
    // Create backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    if (!backend || !backend->Initialize())
    {
        printf("Failed to initialize backend\n");
        delete backend;
        return;
    }
    
    // Create shader manager
    ShaderManager shaderManager(backend);
    shaderManager.Initialize();
    
    // Create modern renderer
    ModernRenderer renderer(backend, &shaderManager);
    if (!renderer.Initialize(1920, 1080))
    {
        printf("Failed to initialize modern renderer\n");
        shaderManager.Shutdown();
        backend->Shutdown();
        delete backend;
        return;
    }
    
    // Add some lights
    printf("Adding lights to scene:\n");
    Light light1;
    light1.position[0] = 0.0f;
    light1.position[1] = 5.0f;
    light1.position[2] = 0.0f;
    light1.color[0] = 1.0f;
    light1.color[1] = 1.0f;
    light1.color[2] = 1.0f;
    light1.intensity = 2.0f;
    light1.radius = 15.0f;
    renderer.AddLight(light1);
    printf("  Added point light at (0, 5, 0)\n");
    
    Light light2;
    light2.position[0] = 10.0f;
    light2.position[1] = 5.0f;
    light2.position[2] = 5.0f;
    light2.color[0] = 1.0f;
    light2.color[1] = 0.5f;
    light2.color[2] = 0.2f;
    light2.intensity = 1.5f;
    light2.radius = 10.0f;
    renderer.AddLight(light2);
    printf("  Added orange point light at (10, 5, 5)\n\n");
    
    // Simulate a render frame with deferred rendering
    printf("Rendering frame:\n");
    
    // Geometry pass
    renderer.BeginGeometryPass();
    printf("  Rendering geometry to G-Buffer...\n");
    // Render all meshes here
    renderer.EndGeometryPass();
    
    // Lighting pass
    renderer.BeginLightingPass();
    printf("  Computing lighting from G-Buffer...\n");
    // Process all lights
    renderer.EndLightingPass();
    
    printf("\n");
    
    // Cleanup
    renderer.Shutdown();
    shaderManager.Shutdown();
    backend->Shutdown();
    delete backend;
    
    printf("Example complete!\n\n");
}

// Example: Shadow mapping
void ExampleShadowMapping()
{
    printf("=== Example: Shadow Mapping ===\n\n");
    
    // Create backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    if (!backend || !backend->Initialize())
    {
        printf("Failed to initialize backend\n");
        delete backend;
        return;
    }
    
    // Create shader manager
    ShaderManager shaderManager(backend);
    shaderManager.Initialize();
    
    // Create modern renderer
    ModernRenderer renderer(backend, &shaderManager);
    renderer.Initialize(1920, 1080);
    
    printf("Rendering with shadows:\n\n");
    
    // Shadow map pass
    renderer.BeginShadowMapPass();
    printf("  Rendering depth from light's perspective...\n");
    // Render all shadow-casting objects
    renderer.EndShadowMapPass();
    
    // Main rendering pass with shadows
    printf("  Rendering scene with shadow mapping shader...\n");
    shaderManager.BindShader(SHADER_TYPE_SHADOW_MAPPING);
    
    // Set light space matrix and shadow map texture
    printf("  Binding shadow map texture\n");
    printf("  Setting light space transformation\n");
    
    printf("\n");
    
    // Cleanup
    renderer.Shutdown();
    shaderManager.Shutdown();
    backend->Shutdown();
    delete backend;
    
    printf("Example complete!\n\n");
}

// Example: Post-processing effects
void ExamplePostProcessing()
{
    printf("=== Example: Post-Processing Effects ===\n\n");
    
    // Create backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    if (!backend || !backend->Initialize())
    {
        printf("Failed to initialize backend\n");
        delete backend;
        return;
    }
    
    // Create shader manager
    ShaderManager shaderManager(backend);
    shaderManager.Initialize();
    
    // Create modern renderer
    ModernRenderer renderer(backend, &shaderManager);
    renderer.Initialize(1920, 1080);
    
    // Configure post-processing
    printf("Configuring post-processing effects:\n");
    PostProcessConfig& config = renderer.GetPostProcessConfig();
    config.enableBloom = true;
    config.enableSSAO = true;
    config.enableHDR = true;
    config.bloomStrength = 0.05f;
    config.exposure = 1.2f;
    printf("  Bloom: enabled (strength: %.2f)\n", config.bloomStrength);
    printf("  SSAO: enabled\n");
    printf("  HDR tone mapping: enabled (exposure: %.2f)\n", config.exposure);
    printf("\n");
    
    // Apply post-processing
    printf("Applying post-processing:\n");
    renderer.ApplyPostProcessing(nullptr, nullptr);
    
    printf("\n");
    
    // Cleanup
    renderer.Shutdown();
    shaderManager.Shutdown();
    backend->Shutdown();
    delete backend;
    
    printf("Example complete!\n\n");
}

int main()
{
    printf("===========================================\n");
    printf("Modern Rendering System Examples\n");
    printf("===========================================\n\n");
    
    // Run all examples
    ExampleBasicShaders();
    ExamplePBRRendering();
    ExampleDeferredRendering();
    ExampleShadowMapping();
    ExamplePostProcessing();
    
    printf("===========================================\n");
    printf("All examples completed successfully!\n");
    printf("===========================================\n");
    
    return 0;
}

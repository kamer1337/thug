# Modern Graphics System Documentation

This document describes the modern graphics abstraction layer, OpenGL 3.3+ backend, shader system, and modern rendering techniques implemented for the THUG engine.

## Table of Contents

1. [Overview](#overview)
2. [Graphics Abstraction Layer](#graphics-abstraction-layer)
3. [OpenGL 3.3+ Backend](#opengl-33-backend)
4. [Shader System](#shader-system)
5. [Modern Rendering Techniques](#modern-rendering-techniques)
6. [Usage Examples](#usage-examples)
7. [Performance Considerations](#performance-considerations)
8. [Future Enhancements](#future-enhancements)

## Overview

The modern graphics system provides a unified, backend-agnostic rendering API that supports modern rendering techniques including:

- **Physically Based Rendering (PBR)**
- **Deferred Rendering**
- **Shadow Mapping**
- **Post-Processing Effects (Bloom, SSAO, HDR)**
- **Modern Shader Pipeline**

### Key Components

```
Code/Gfx/Backend/
├── GraphicsBackend.h/cpp      # Core abstraction layer
├── OpenGLBackend.h/cpp        # OpenGL 3.3+ implementation
├── ShaderLibrary.h            # Predefined modern shaders
├── ShaderManager.h/cpp        # Shader compilation and caching
└── ModernRenderer.h/cpp       # Modern rendering techniques
```

## Graphics Abstraction Layer

### Architecture

The graphics backend provides a unified interface (`IGraphicsBackend`) that abstracts platform-specific graphics APIs:

```
IGraphicsBackend (Interface)
    ├── OpenGLBackend (OpenGL 3.3+)
    ├── VulkanBackend (Vulkan - separate implementation)
    └── StubBackend (Testing/Headless)
```

### Core Features

#### Resource Management
- **Textures**: Create, bind, update, and destroy textures
- **Meshes**: Vertex buffer objects (VBO), vertex array objects (VAO)
- **Shaders**: GLSL shader compilation and linking

#### Render State Management
- Depth testing and writing
- Face culling
- Blending modes
- Viewport control

#### Matrix Operations
- Projection matrix
- View matrix
- Model matrix

### Basic Usage

```cpp
#include <Gfx/Backend/GraphicsBackend.h>

using namespace Gfx::Backend;

// Create and initialize backend
IGraphicsBackend* backend = CreateBackend("opengl");
backend->Initialize();

// Set viewport
backend->SetViewport(0, 0, 1920, 1080);

// Create texture
uint8 textureData[256 * 256 * 4];
Texture* tex = backend->CreateTexture(256, 256, TEXTURE_FORMAT_RGBA8, textureData);

// Render frame
backend->BeginFrame();
backend->Clear(CLEAR_COLOR | CLEAR_DEPTH, 0x000000FF);

backend->BindTexture(tex, 0);
// Draw geometry...

backend->EndFrame();
backend->SwapBuffers();

// Cleanup
backend->DestroyTexture(tex);
backend->Shutdown();
delete backend;
```

## OpenGL 3.3+ Backend

### Implementation Details

The OpenGL backend (`OpenGLBackend`) implements the `IGraphicsBackend` interface using OpenGL 3.3 Core Profile features:

#### Features Implemented

1. **Modern OpenGL Pipeline**
   - Vertex Array Objects (VAO)
   - Vertex Buffer Objects (VBO)
   - Element Buffer Objects (EBO)
   - Shader programs (GLSL 330)

2. **Texture Management**
   - 2D textures with multiple formats (RGBA8, RGB8, BGRA8, DXT1/3/5)
   - Texture filtering (linear, nearest)
   - Texture wrapping modes

3. **Shader System**
   - Vertex and fragment shader compilation
   - Program linking with error reporting
   - Uniform management

4. **State Management**
   - Depth testing and functions
   - Face culling (front, back, none)
   - Blending with custom blend functions
   - Viewport management

### Platform Support

- **Windows**: Uses WGL for extension loading
- **Linux**: Uses GLX with `GL_GLEXT_PROTOTYPES`
- **macOS**: Native OpenGL support

### Compilation

Enable OpenGL backend at build time:

```bash
cmake -DUSE_OPENGL_RENDERER=ON ..
cmake --build .
```

## Shader System

### Shader Library

The `ShaderLibrary.h` provides predefined GLSL 330 shaders for common rendering tasks:

#### Basic Shaders
- **Basic Vertex/Fragment**: Simple transformation with color and texture

#### Lighting Shaders
- **Phong Vertex/Fragment**: Classic Phong lighting model
  - Ambient, diffuse, specular components
  - Material properties
  - Multiple light support

#### PBR Shaders
- **PBR Vertex/Fragment**: Physically Based Rendering
  - Cook-Torrance BRDF
  - Normal Distribution Function (GGX)
  - Geometry Function (Schlick-GGX)
  - Fresnel-Schlick approximation
  - Metallic-roughness workflow
  - Normal mapping support

#### Shadow Mapping Shaders
- **Shadow Map Vertex/Fragment**: Depth rendering from light's perspective
- **Shadow Mapping Vertex/Fragment**: Scene rendering with PCF soft shadows

#### Post-Processing Shaders
- **Screen Quad Vertex**: Full-screen quad for post-processing
- **Bloom Fragment**: HDR bloom with configurable strength
- **Gaussian Blur Fragment**: Separable Gaussian blur
- **SSAO Fragment**: Screen-space ambient occlusion

### Shader Manager

The `ShaderManager` class provides high-level shader management:

#### Features

1. **Automatic Compilation**: Compiles and links shaders automatically
2. **Caching**: Maintains a cache of compiled shaders
3. **Error Reporting**: Detailed compilation/linking error messages
4. **Predefined Shaders**: Easy access to common shaders by type
5. **Custom Shaders**: Load shaders from files or source strings
6. **Uniform Helpers**: Simplified uniform setting

#### Usage

```cpp
#include <Gfx/Backend/ShaderManager.h>

using namespace Gfx::Backend;

// Create shader manager
ShaderManager shaderManager(backend);
shaderManager.Initialize();  // Creates all predefined shaders

// Get predefined shader
ShaderProgram* phongShader = shaderManager.GetShader(SHADER_TYPE_PHONG);

// Bind and use shader
shaderManager.BindShader(SHADER_TYPE_PHONG);
shaderManager.SetUniformVec3("uLightPos", 5.0f, 10.0f, 5.0f);
shaderManager.SetUniformVec3("uLightColor", 1.0f, 1.0f, 1.0f);
shaderManager.SetUniformFloat("uShininess", 32.0f);

// Create custom shader
ShaderProgram* customShader = shaderManager.CreateShader(
    "myshader", vertexSource, fragmentSource);

// Load shader from files
ShaderProgram* fileShader = shaderManager.LoadShader(
    "terrain", "shaders/terrain.vert", "shaders/terrain.frag");
```

## Modern Rendering Techniques

### Modern Renderer

The `ModernRenderer` class implements advanced rendering techniques:

#### Deferred Rendering

Deferred rendering separates geometry and lighting into two passes:

1. **Geometry Pass**: Renders scene geometry to G-Buffer
   - Position texture (RGB16F)
   - Normal texture (RGB16F)
   - Albedo + Specular texture (RGBA8)
   - Depth texture

2. **Lighting Pass**: Computes lighting using G-Buffer data
   - Supports many lights efficiently
   - Per-pixel lighting
   - Advanced material models

**Usage:**

```cpp
ModernRenderer renderer(backend, shaderManager);
renderer.Initialize(1920, 1080);

// Add lights
Light light;
light.position[0] = 0.0f;
light.position[1] = 5.0f;
light.position[2] = 0.0f;
light.color[0] = 1.0f;
light.color[1] = 1.0f;
light.color[2] = 1.0f;
light.intensity = 2.0f;
renderer.AddLight(light);

// Render frame
renderer.BeginGeometryPass();
// Render all geometry
renderer.EndGeometryPass();

renderer.BeginLightingPass();
// Lighting computed automatically
renderer.EndLightingPass();
```

#### Shadow Mapping

Implements shadow mapping with percentage closer filtering (PCF):

**Features:**
- Configurable shadow map resolution (default: 2048x2048)
- Bias to prevent shadow acne
- PCF for soft shadow edges
- Multiple light support

**Usage:**

```cpp
// Shadow pass
renderer.BeginShadowMapPass();
// Render shadow-casting geometry
renderer.EndShadowMapPass();

// Main pass with shadows
shaderManager.BindShader(SHADER_TYPE_SHADOW_MAPPING);
// Bind shadow map
backend->BindTexture(renderer.GetShadowMap(), 1);
// Render scene
```

#### Post-Processing Effects

##### Bloom
HDR bloom effect with configurable threshold and strength:

```cpp
PostProcessConfig& config = renderer.GetPostProcessConfig();
config.enableBloom = true;
config.bloomThreshold = 1.0f;
config.bloomStrength = 0.04f;
```

##### SSAO (Screen-Space Ambient Occlusion)
Adds realistic ambient occlusion:

```cpp
config.enableSSAO = true;
config.ssaoRadius = 0.5f;
config.ssaoBias = 0.025f;
```

##### HDR Tone Mapping
Converts HDR colors to displayable range:

```cpp
config.enableHDR = true;
config.exposure = 1.0f;
```

#### PBR Rendering

Physically Based Rendering with metallic-roughness workflow:

**Features:**
- Cook-Torrance BRDF
- Multiple light sources
- Normal mapping
- Metallic and roughness maps
- Ambient occlusion maps

**Usage:**

```cpp
shaderManager.BindShader(SHADER_TYPE_PBR);

// Set material properties
shaderManager.SetUniformVec3("uAlbedo", 0.8f, 0.2f, 0.2f);
shaderManager.SetUniformFloat("uMetallic", 0.3f);
shaderManager.SetUniformFloat("uRoughness", 0.7f);
shaderManager.SetUniformBool("uUseTextures", false);

// Set lights (up to 4)
// Render mesh
```

## Usage Examples

### Complete Rendering Setup

```cpp
#include <Gfx/Backend/GraphicsBackend.h>
#include <Gfx/Backend/ShaderManager.h>
#include <Gfx/Backend/ModernRenderer.h>

using namespace Gfx::Backend;

int main()
{
    // Initialize backend
    IGraphicsBackend* backend = CreateBackend("opengl");
    backend->Initialize();
    backend->SetViewport(0, 0, 1920, 1080);
    
    // Initialize shader manager
    ShaderManager shaderManager(backend);
    shaderManager.Initialize();
    
    // Initialize modern renderer
    ModernRenderer renderer(backend, &shaderManager);
    renderer.Initialize(1920, 1080);
    
    // Configure post-processing
    PostProcessConfig& config = renderer.GetPostProcessConfig();
    config.enableBloom = true;
    config.enableSSAO = true;
    config.enableHDR = true;
    
    // Add lights
    Light mainLight;
    mainLight.position[0] = 0.0f;
    mainLight.position[1] = 10.0f;
    mainLight.position[2] = 0.0f;
    mainLight.intensity = 2.0f;
    renderer.AddLight(mainLight);
    
    // Game loop
    while (running)
    {
        // Shadow pass
        renderer.BeginShadowMapPass();
        // Render shadow casters
        renderer.EndShadowMapPass();
        
        // Geometry pass (deferred)
        renderer.BeginGeometryPass();
        // Render all geometry
        renderer.EndGeometryPass();
        
        // Lighting pass
        renderer.BeginLightingPass();
        renderer.EndLightingPass();
        
        // Post-processing
        renderer.ApplyPostProcessing(nullptr, nullptr);
        
        backend->SwapBuffers();
    }
    
    // Cleanup
    renderer.Shutdown();
    shaderManager.Shutdown();
    backend->Shutdown();
    delete backend;
    
    return 0;
}
```

### Using Different Shaders

```cpp
// Basic rendering
shaderManager.BindShader(SHADER_TYPE_BASIC);
shaderManager.SetUniformVec4("uColor", 1.0f, 0.0f, 0.0f, 1.0f);

// Phong lighting
shaderManager.BindShader(SHADER_TYPE_PHONG);
shaderManager.SetUniformVec3("uLightPos", 5.0f, 10.0f, 5.0f);
shaderManager.SetUniformFloat("uShininess", 32.0f);

// PBR rendering
shaderManager.BindShader(SHADER_TYPE_PBR);
shaderManager.SetUniformFloat("uMetallic", 0.5f);
shaderManager.SetUniformFloat("uRoughness", 0.3f);
```

## Performance Considerations

### Optimization Tips

1. **Batch Rendering**: Group objects by material and shader to minimize state changes
2. **Texture Atlases**: Combine multiple textures to reduce texture binding
3. **Level of Detail (LOD)**: Use simpler shaders for distant objects
4. **Frustum Culling**: Only render visible geometry
5. **Deferred Rendering**: Use for scenes with many lights
6. **Shadow Map Resolution**: Balance quality vs performance (1024-4096)

### Profiling

Key metrics to monitor:
- Draw calls per frame
- Shader switches per frame
- Texture bindings per frame
- Shadow map render time
- Post-processing time

### Platform-Specific Notes

- **Windows**: OpenGL driver quality varies; test on multiple GPUs
- **Linux**: Mesa drivers support OpenGL 3.3+ on most hardware
- **macOS**: Limited to OpenGL 4.1; consider Metal for future

## Future Enhancements

### Planned Features

1. **Additional Backends**
   - DirectX 11/12 for Windows
   - Metal for macOS/iOS
   - Vulkan integration improvements

2. **Advanced Rendering**
   - Cascaded shadow maps for large scenes
   - Temporal anti-aliasing (TAA)
   - Screen-space reflections (SSR)
   - Global illumination approximations

3. **Shader Improvements**
   - Shader hot-reloading for development
   - Shader permutation system
   - Compute shader support
   - Tessellation shaders

4. **Performance**
   - Multi-threaded command buffer generation
   - Instanced rendering support
   - Indirect rendering
   - GPU-driven rendering

5. **Tools**
   - Shader debugger integration
   - Visual shader editor
   - Render target visualization
   - Performance profiler

## References

### OpenGL Resources
- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [Learn OpenGL](https://learnopengl.com/)
- [OpenGL Wiki](https://www.khronos.org/opengl/wiki/)

### PBR Resources
- [PBR Theory (Disney)](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)
- [Physically Based Rendering Book](http://www.pbr-book.org/)

### Shader Programming
- [The Book of Shaders](https://thebookofshaders.com/)
- [Shader Toy](https://www.shadertoy.com/)

### Deferred Rendering
- [Deferred Shading Tutorial](http://ogldev.atspace.co.uk/)

## Troubleshooting

### Common Issues

1. **Shader Compilation Errors**
   - Check GLSL version compatibility (330)
   - Verify uniform names and types
   - Enable shader error logging

2. **Black Screen**
   - Verify OpenGL context creation
   - Check shader compilation status
   - Validate framebuffer completeness

3. **Performance Issues**
   - Profile draw calls and state changes
   - Reduce shadow map resolution
   - Disable expensive post-processing effects

4. **Z-Fighting**
   - Adjust near/far planes in projection matrix
   - Use depth buffer with sufficient precision
   - Apply small polygon offset

## License

Part of the THUG source code repository. See main LICENSE for details.

## Contributing

See [CONTRIBUTING.md](../../CONTRIBUTING.md) for contribution guidelines.

---

*Last updated: December 2024*

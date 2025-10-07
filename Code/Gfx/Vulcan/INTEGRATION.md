# Vulcan Renderer Integration Guide

## Overview

This document describes how to integrate the Vulcan (Vulkan) renderer into THUG. The Vulcan renderer is a modern graphics backend designed to replace platform-specific rendering implementations.

## Architecture

The Vulcan renderer follows a modular design that separates platform-independent rendering logic from platform-specific Vulkan API calls:

```
Application Layer
      ↓
Nx::CEngine (Platform-independent engine)
      ↓
Nx::CVulcanModel (Platform-specific model)
      ↓
NxVulcan::render_scene() (Rendering functions)
      ↓
Vulkan API (GPU operations)
```

## Integration Steps

### 1. Platform Selection

The platform-specific model class is selected at compile time based on platform defines. To use the Vulcan renderer, the build system should:

```cmake
# In CMakeLists.txt
if(USE_VULKAN_RENDERER)
    add_definitions(-DUSE_VULKAN_RENDERER)
    include_directories(${CMAKE_SOURCE_DIR}/Code/Gfx/Vulcan)
endif()
```

### 2. Model Creation

When creating a model, use the `CVulcanModel` class:

```cpp
#include <gfx/Vulcan/p_nxmodel.h>

// Create a Vulkan-backed model
Nx::CModel* pModel = new Nx::CVulcanModel();
```

### 3. Initialization

Initialize the Vulkan renderer before creating any scenes or models:

```cpp
#include <gfx/Vulcan/NX/render.h>

// Initialize Vulkan renderer
if (!NxVulcan::init_vulkan())
{
    // Handle initialization failure
    return false;
}
```

### 4. Scene Management

Create and manage scenes using the Vulcan renderer API:

```cpp
// Create a scene
NxVulcan::sScene* pScene = NxVulcan::create_scene();

// Create meshes
NxVulcan::sMesh* pMesh = NxVulcan::create_mesh(checksum);

// Configure mesh data
pMesh->m_num_vertices = numVertices;
pMesh->mp_positions = new float[numVertices * 3];
// ... set up vertex data ...

// Upload mesh data to GPU
NxVulcan::upload_mesh_data(pMesh);

// Add mesh to scene
NxVulcan::add_mesh_to_scene(pScene, pMesh);
```

### 5. Texture Management

Load and manage textures:

```cpp
// Create a texture from data
NxVulcan::sTexture* pTexture = NxVulcan::create_texture(
    checksum, 
    width, 
    height, 
    NxVulcan::sTexture::TEXTURE_FORMAT_DXT1, 
    pTextureData
);

// Or load from file
NxVulcan::sTexture* pTexture = NxVulcan::load_texture("path/to/texture.tex");

// Retrieve texture by checksum
NxVulcan::sTexture* pTex = NxVulcan::get_texture(checksum);
```

### 6. Camera Setup

Configure the camera before rendering:

```cpp
Mth::Matrix viewMatrix;
Mth::Vector cameraPos(0.0f, 10.0f, 20.0f, 1.0f);
float fieldOfView = 60.0f;  // degrees
float aspectRatio = 16.0f / 9.0f;

NxVulcan::set_camera(&viewMatrix, &cameraPos, fieldOfView, aspectRatio);
```

### 7. Rendering

Render the scene with appropriate flags:

```cpp
// Render opaque and semi-transparent geometry
NxVulcan::render_scene(
    pScene, 
    NxVulcan::vRENDER_OPAQUE | NxVulcan::vRENDER_SEMITRANSPARENT
);
```

### 8. Cleanup

Properly shut down the renderer on exit:

```cpp
// Destroy scene
NxVulcan::destroy_scene(pScene);

// Shutdown renderer
NxVulcan::shutdown_vulkan();
```

## Render Flags

The renderer supports various rendering modes:

| Flag | Description |
|------|-------------|
| `vRENDER_OPAQUE` | Render opaque geometry |
| `vRENDER_SEMITRANSPARENT` | Render semi-transparent geometry |
| `vRENDER_OCCLUDED` | Render occluded geometry |
| `vRENDER_NO_CULLING` | Disable frustum culling |
| `vRENDER_SORT_FRONT_TO_BACK` | Sort geometry front-to-back |
| `vRENDER_SHADOW_VOLUMES` | Render shadow volumes |
| `vRENDER_BILLBOARDS` | Render billboard sprites |

## Blend Modes

The renderer supports multiple blend modes for transparency and effects:

```cpp
NxVulcan::set_blend_mode(NxVulcan::vBLEND_MODE_BLEND);
```

Available blend modes:
- `vBLEND_MODE_DIFFUSE` - Standard diffuse rendering
- `vBLEND_MODE_ADD` - Additive blending
- `vBLEND_MODE_BLEND` - Alpha blending
- `vBLEND_MODE_MODULATE` - Multiplicative blending
- `vBLEND_MODE_BRIGHTEN` - Brightening effect
- And more (see `render.h` for full list)

## Render States

Configure rendering pipeline states:

```cpp
// Enable/disable depth testing
NxVulcan::set_render_state(RS_ZTESTENABLE, 1);

// Enable/disable depth writing
NxVulcan::set_render_state(RS_ZWRITEENABLE, 1);

// Set alpha cutoff threshold (0-255)
NxVulcan::set_render_state(RS_ALPHACUTOFF, 128);

// Set UV addressing mode
NxVulcan::set_render_state(RS_UVADDRESSMODE0, 0);  // Wrap
```

## Texture Formats

The renderer supports multiple texture formats for different platforms:

### Console Formats
- `TEXTURE_FORMAT_RGBA32` - 32-bit RGBA uncompressed
- `TEXTURE_FORMAT_RGB24` - 24-bit RGB uncompressed
- `TEXTURE_FORMAT_PALETTE8` - 8-bit paletted
- `TEXTURE_FORMAT_PALETTE4` - 4-bit paletted

### PC Formats (DirectX/Vulkan)
- `TEXTURE_FORMAT_DXT1` - BC1 compressed (4:1 ratio)
- `TEXTURE_FORMAT_DXT3` - BC2 compressed with explicit alpha
- `TEXTURE_FORMAT_DXT5` - BC3 compressed with interpolated alpha
- `TEXTURE_FORMAT_A8R8G8B8` - 32-bit ARGB uncompressed
- `TEXTURE_FORMAT_R5G6B5` - 16-bit RGB
- `TEXTURE_FORMAT_A1R5G5B5` - 16-bit ARGB with 1-bit alpha
- `TEXTURE_FORMAT_A4R4G4B4` - 16-bit ARGB with 4-bit alpha

## Vulkan-Specific Features

### Texture Projection

The renderer supports projecting textures onto geometry (for effects like shadows, spotlights):

```cpp
// Create projection details
NxVulcan::create_texture_projection_details(pTexture, pModel, pScene);

// Set projection camera
Mth::Vector projPos(0.0f, 100.0f, 0.0f);
Mth::Vector projTarget(0.0f, 0.0f, 0.0f);
NxVulcan::set_texture_projection_camera(pTexture, &projPos, &projTarget);

// Cleanup
NxVulcan::destroy_texture_projection_details(pTexture);
```

### Frustum Culling

The renderer includes frustum culling for visibility optimization:

```cpp
// Set frustum transform
Mth::Matrix frustumTransform;
NxVulcan::set_frustum_bbox_transform(&frustumTransform);

// Check sphere visibility
Mth::Vector sphereCenter(0.0f, 0.0f, 0.0f);
float radius = 10.0f;
bool visible = NxVulcan::frustum_check_sphere(&sphereCenter, radius);

// Or use the simplified visibility check
bool visible = NxVulcan::IsVisible(sphereCenter, radius);
```

## Current Implementation Status

The Vulcan renderer is currently implemented as a **software architecture** with full data structures and control flow, but GPU operations are stubbed out. 

### Fully Implemented
✅ All function signatures and interfaces
✅ Data structure definitions (sTexture, sMesh, sScene, sMaterial)
✅ Hash table management for textures and projection details
✅ Scene graph management (create, destroy, add mesh)
✅ Mesh lifecycle (create, destroy, add to scene)
✅ Texture lifecycle (create, load, destroy, retrieve)
✅ Camera and frustum setup
✅ Render state tracking
✅ Blend mode management
✅ Visibility and culling logic

### Stub Implementations (Marked "In a full implementation")
🔨 Vulkan instance creation
🔨 Physical/logical device selection
🔨 Swapchain management
🔨 Command buffer recording
🔨 Pipeline state objects
🔨 Descriptor sets
🔨 Actual GPU buffer creation
🔨 Actual texture upload
🔨 Draw call submission

### To Complete the Renderer

To make this a fully functional Vulkan renderer, you would need to:

1. **Initialize Vulkan Context** (`init_vulkan`)
   - Create VkInstance
   - Select VkPhysicalDevice
   - Create VkDevice and queues
   - Create VkSwapchain

2. **Implement Buffer Management** (`upload_mesh_data`)
   - Create VkBuffer for vertices and indices
   - Allocate and bind device memory
   - Copy data using staging buffers

3. **Implement Texture Management** (`create_texture`, `load_texture`)
   - Create VkImage
   - Allocate and bind image memory
   - Create VkImageView and VkSampler
   - Handle format conversions

4. **Implement Rendering** (`render_scene`)
   - Record VkCommandBuffer
   - Begin/end render pass
   - Bind pipeline, descriptors, buffers
   - Issue draw calls
   - Submit to queue and present

5. **Implement Resource Cleanup**
   - Destroy all Vulkan objects properly
   - Wait for device idle before cleanup

## Platform Integration

### Engine Integration

To integrate with the main engine (`Nx::CEngine`), modify the platform-specific functions:

```cpp
// In nx.cpp or platform-specific implementation file

CModel* CEngine::s_plat_init_model()
{
    #ifdef USE_VULKAN_RENDERER
        return new CVulcanModel();
    #else
        // Fall back to Win32 or other platform
        return new CWin32Model();
    #endif
}
```

### Build System Integration

Add Vulkan-specific files to the build:

```cmake
# CMakeLists.txt
if(USE_VULKAN_RENDERER)
    file(GLOB VULKAN_SOURCES
        "${CMAKE_SOURCE_DIR}/Code/Gfx/Vulcan/*.cpp"
        "${CMAKE_SOURCE_DIR}/Code/Gfx/Vulcan/NX/*.cpp"
    )
    list(APPEND ALL_SOURCES ${VULKAN_SOURCES})
    
    # Link Vulkan library when available
    find_package(Vulkan)
    if(Vulkan_FOUND)
        target_link_libraries(thug ${Vulkan_LIBRARIES})
        target_include_directories(thug PRIVATE ${Vulkan_INCLUDE_DIRS})
    endif()
endif()
```

## Testing

### Unit Tests

Example test for basic functionality:

```cpp
#include <gtest/gtest.h>
#include <gfx/Vulcan/NX/render.h>

TEST(VulcanRenderer, Initialization)
{
    EXPECT_TRUE(NxVulcan::init_vulkan());
    NxVulcan::shutdown_vulkan();
}

TEST(VulcanRenderer, SceneCreation)
{
    NxVulcan::init_vulkan();
    
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    EXPECT_NE(pScene, nullptr);
    EXPECT_EQ(pScene->m_num_meshes, 0);
    
    NxVulcan::destroy_scene(pScene);
    NxVulcan::shutdown_vulkan();
}

TEST(VulcanRenderer, MeshManagement)
{
    NxVulcan::init_vulkan();
    
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    NxVulcan::sMesh* pMesh = NxVulcan::create_mesh(0x12345678);
    
    NxVulcan::add_mesh_to_scene(pScene, pMesh);
    EXPECT_EQ(pScene->m_num_meshes, 1);
    
    NxVulcan::destroy_mesh(pMesh);
    NxVulcan::destroy_scene(pScene);
    NxVulcan::shutdown_vulkan();
}
```

## Performance Considerations

### Best Practices

1. **Batch Rendering**: Group meshes by material to minimize state changes
2. **Frustum Culling**: Use `frustum_check_sphere` before rendering
3. **Texture Management**: Reuse textures via checksum lookup
4. **Memory Management**: Upload mesh data once, reuse buffers
5. **Blend Mode**: Render opaque first, then transparent (back-to-front)

### Typical Frame Rendering

```cpp
void RenderFrame()
{
    // 1. Update camera
    UpdateCamera();
    NxVulcan::set_camera(&viewMatrix, &cameraPos, fov, aspectRatio);
    
    // 2. Cull invisible objects
    for (auto* pMesh : allMeshes)
    {
        if (NxVulcan::IsVisible(pMesh->m_sphere, radius))
        {
            visibleMeshes.push_back(pMesh);
        }
    }
    
    // 3. Render opaque geometry
    NxVulcan::render_scene(pScene, NxVulcan::vRENDER_OPAQUE);
    
    // 4. Render transparent geometry (back-to-front sorted)
    NxVulcan::render_scene(pScene, NxVulcan::vRENDER_SEMITRANSPARENT);
    
    // 5. Render special effects
    NxVulcan::render_light_glows(true);
    NxVulcan::render_shadow_targets();
}
```

## Debugging

### Debug Utilities

The renderer respects debug builds:

```cpp
#ifdef __NOPT_DEBUG__
    // Debug mode - extra validation and logging
    Dbg_MsgAssert(pScene != NULL, ("Scene is NULL!"));
#endif
```

### Common Issues

1. **Black Screen**: Check that `init_vulkan()` succeeded
2. **Missing Geometry**: Verify mesh was added to scene and uploaded
3. **Wrong Colors**: Check blend mode and material settings
4. **Performance Issues**: Enable frustum culling, batch by material

## Future Enhancements

Potential improvements for the Vulcan renderer:

1. **Compute Shaders**: For particle systems and post-processing
2. **Multi-threading**: Command buffer recording on multiple threads
3. **Dynamic Resolution**: Adaptive resolution scaling
4. **Ray Tracing**: RTX support for reflections and shadows
5. **HDR Rendering**: High dynamic range pipeline
6. **Async Compute**: Overlap compute and graphics work

## References

- `Code/Gfx/Vulcan/NX/render.h` - API reference
- `Code/Gfx/Vulcan/NX/render.cpp` - Implementation
- `Code/Gfx/Vulcan/README.md` - Architecture overview
- `Code/Gfx/Vulcan/IMPLEMENTATION.md` - Implementation details
- [Vulkan Specification](https://www.khronos.org/vulkan/)
- [Vulkan Tutorial](https://vulkan-tutorial.com/)

## Support

For questions or issues with the Vulcan renderer:
1. Check existing documentation in `Code/Gfx/Vulcan/`
2. Review the implementation in `render.cpp`
3. Consult Vulkan specification for API details
4. Open an issue on the repository

---

**Note**: This renderer is currently a software architecture. Full Vulkan GPU functionality requires implementing the stub operations marked with "In a full implementation" comments throughout the codebase.

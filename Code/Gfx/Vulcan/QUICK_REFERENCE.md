# Vulcan Renderer - Quick Reference

Quick API reference for the Vulcan (Vulkan) renderer. For detailed documentation, see [INTEGRATION.md](INTEGRATION.md).

## Initialization & Cleanup

```cpp
#include <gfx/Vulcan/NX/render.h>

// Initialize renderer
bool success = NxVulcan::init_vulkan();

// Shutdown renderer
NxVulcan::shutdown_vulkan();
```

## Scene Management

```cpp
// Create scene
NxVulcan::sScene* pScene = NxVulcan::create_scene();

// Destroy scene
NxVulcan::destroy_scene(pScene);
```

## Mesh Operations

```cpp
// Create mesh
NxVulcan::sMesh* pMesh = NxVulcan::create_mesh(checksum);

// Configure mesh
pMesh->m_num_vertices = 3;
pMesh->mp_positions = new float[9] { -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };

// Upload to GPU
NxVulcan::upload_mesh_data(pMesh);

// Add to scene
NxVulcan::add_mesh_to_scene(pScene, pMesh);

// Destroy mesh
NxVulcan::destroy_mesh(pMesh);
```

## Texture Operations

```cpp
// Create texture
NxVulcan::sTexture* pTex = NxVulcan::create_texture(
    checksum, width, height, format, pData
);

// Load texture from file
NxVulcan::sTexture* pTex = NxVulcan::load_texture("path/to/texture.tex");

// Get texture by checksum
NxVulcan::sTexture* pTex = NxVulcan::get_texture(checksum);

// Destroy texture
NxVulcan::destroy_texture(pTex);
```

## Camera Setup

```cpp
Mth::Matrix viewMatrix;
Mth::Vector cameraPos(0.0f, 5.0f, 10.0f, 1.0f);
float fov = 60.0f;
float aspect = 16.0f / 9.0f;

NxVulcan::set_camera(&viewMatrix, &cameraPos, fov, aspect);
```

## Rendering

```cpp
// Render opaque geometry
NxVulcan::render_scene(pScene, NxVulcan::vRENDER_OPAQUE);

// Render transparent geometry
NxVulcan::render_scene(pScene, NxVulcan::vRENDER_SEMITRANSPARENT);

// Render both
NxVulcan::render_scene(pScene, 
    NxVulcan::vRENDER_OPAQUE | NxVulcan::vRENDER_SEMITRANSPARENT);
```

## Render States

```cpp
// Depth testing
NxVulcan::set_render_state(RS_ZTESTENABLE, 1);
NxVulcan::set_render_state(RS_ZWRITEENABLE, 1);

// Alpha cutoff (0-255)
NxVulcan::set_render_state(RS_ALPHACUTOFF, 128);

// Blend mode
NxVulcan::set_blend_mode(NxVulcan::vBLEND_MODE_BLEND);
```

## Visibility Culling

```cpp
// Check sphere visibility
Mth::Vector center(0.0f, 0.0f, 0.0f);
float radius = 10.0f;
bool visible = NxVulcan::IsVisible(center, radius);

// Or use frustum check directly
bool visible = NxVulcan::frustum_check_sphere(&center, radius);
```

## Texture Formats

### Console Formats
- `TEXTURE_FORMAT_RGBA32` - 32-bit RGBA
- `TEXTURE_FORMAT_RGB24` - 24-bit RGB
- `TEXTURE_FORMAT_PALETTE8` - 8-bit paletted
- `TEXTURE_FORMAT_PALETTE4` - 4-bit paletted

### PC Formats
- `TEXTURE_FORMAT_DXT1` - BC1 compressed
- `TEXTURE_FORMAT_DXT3` - BC2 compressed
- `TEXTURE_FORMAT_DXT5` - BC3 compressed
- `TEXTURE_FORMAT_A8R8G8B8` - 32-bit ARGB
- `TEXTURE_FORMAT_R5G6B5` - 16-bit RGB
- `TEXTURE_FORMAT_A1R5G5B5` - 16-bit ARGB (1-bit alpha)
- `TEXTURE_FORMAT_A4R4G4B4` - 16-bit ARGB (4-bit alpha)

## Render Flags

- `vRENDER_OPAQUE` - Opaque geometry
- `vRENDER_SEMITRANSPARENT` - Semi-transparent geometry
- `vRENDER_OCCLUDED` - Occluded geometry
- `vRENDER_NO_CULLING` - Disable frustum culling
- `vRENDER_SORT_FRONT_TO_BACK` - Front-to-back sorting
- `vRENDER_SHADOW_VOLUMES` - Shadow volumes
- `vRENDER_BILLBOARDS` - Billboard sprites

## Blend Modes

- `vBLEND_MODE_DIFFUSE` - Standard diffuse
- `vBLEND_MODE_ADD` - Additive blending
- `vBLEND_MODE_BLEND` - Alpha blending
- `vBLEND_MODE_MODULATE` - Multiplicative
- `vBLEND_MODE_BRIGHTEN` - Brightening
- `vBLEND_MODE_SUBTRACT` - Subtractive
- And more (see `render.h`)

## Typical Frame Loop

```cpp
void RenderFrame()
{
    // 1. Update camera
    NxVulcan::set_camera(&viewMatrix, &cameraPos, fov, aspect);
    
    // 2. Set render states
    NxVulcan::set_render_state(RS_ZTESTENABLE, 1);
    NxVulcan::set_render_state(RS_ZWRITEENABLE, 1);
    
    // 3. Render opaque
    NxVulcan::render_scene(pScene, NxVulcan::vRENDER_OPAQUE);
    
    // 4. Render transparent
    NxVulcan::render_scene(pScene, NxVulcan::vRENDER_SEMITRANSPARENT);
    
    // 5. Special effects
    NxVulcan::render_light_glows(true);
}
```

## Platform-Specific Model

```cpp
#include <gfx/Vulcan/p_nxmodel.h>

// Create Vulcan model
Nx::CVulcanModel* pModel = new Nx::CVulcanModel();

// Set scene
pModel->SetScene(pScene);

// Get/set bounding sphere
Mth::Vector sphere = pModel->plat_get_bounding_sphere();
pModel->plat_set_bounding_sphere(sphere);

// Cleanup
delete pModel;
```

## Complete Example

```cpp
#include <gfx/Vulcan/NX/render.h>

int main()
{
    // Initialize
    if (!NxVulcan::init_vulkan())
        return 1;
    
    // Create scene and mesh
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    NxVulcan::sMesh* pMesh = NxVulcan::create_mesh(0x12345678);
    
    // Configure mesh
    pMesh->m_num_vertices = 3;
    pMesh->mp_positions = new float[9];
    // ... set vertex data ...
    
    NxVulcan::upload_mesh_data(pMesh);
    NxVulcan::add_mesh_to_scene(pScene, pMesh);
    
    // Set up camera
    Mth::Matrix view;
    Mth::Vector pos(0.0f, 0.0f, 10.0f);
    NxVulcan::set_camera(&view, &pos, 60.0f, 16.0f/9.0f);
    
    // Render
    NxVulcan::render_scene(pScene, 
        NxVulcan::vRENDER_OPAQUE | NxVulcan::vRENDER_SEMITRANSPARENT);
    
    // Cleanup
    NxVulcan::destroy_mesh(pMesh);
    NxVulcan::destroy_scene(pScene);
    NxVulcan::shutdown_vulkan();
    
    return 0;
}
```

## Build Configuration

Enable Vulkan renderer in CMake:

```bash
cmake -DUSE_VULKAN_RENDERER=ON ..
```

## Documentation

- **[README.md](README.md)** - Overview and getting started
- **[INTEGRATION.md](INTEGRATION.md)** - Complete integration guide
- **[IMPLEMENTATION.md](IMPLEMENTATION.md)** - Implementation details
- **[example.cpp](example.cpp)** - 10 complete examples
- **[NX/render.h](NX/render.h)** - Full API reference
- **QUICK_REFERENCE.md** - This file

## Current Status

✅ Software architecture fully implemented
✅ All API functions defined and callable
✅ Data structures complete
✅ Scene and mesh management working
✅ Texture lifecycle complete
✅ Camera and frustum setup functional

🔨 GPU operations stubbed (marked "In a full implementation")
🔨 Requires Vulkan API implementation for actual rendering

See [INTEGRATION.md](INTEGRATION.md) for complete details on implementation status.

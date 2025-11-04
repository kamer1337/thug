# Vulkan Rendering API Reference

## Overview

> **Note:** This implementation provides a complete Vulkan rendering API that works in two modes:
> 1. **Full Vulkan Mode** - When compiled with Vulkan SDK, uses actual Vulkan API calls
> 2. **Stub Mode** - When compiled without Vulkan SDK, provides stub implementations for testing
> 
> Some features are simulated for simplicity (e.g., headless swapchain without WSI). See [Implementation Notes](#implementation-notes) for details.

This document describes the complete Vulkan rendering API implementation for the THUG engine, including:
- Instance and device creation
- Swapchain management
- Command buffer recording
- Pipeline state objects
- Buffer and texture upload
- Draw call submission

## API Categories

### 1. Initialization and Shutdown

#### `bool init_vulkan(void)`
Initializes the Vulkan renderer, including:
- Creating Vulkan instance
- Selecting physical device
- Creating logical device
- Creating command pool

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
if (!NxVulcan::init_vulkan()) {
    printf("Failed to initialize Vulkan\n");
    return false;
}
```

#### `void shutdown_vulkan(void)`
Shuts down the Vulkan renderer and cleans up all resources:
- Command buffers
- Swapchain
- Pipelines
- Render passes
- Device and instance

**Example:**
```cpp
NxVulcan::shutdown_vulkan();
```

### 2. Swapchain Management

#### `bool create_swapchain(void *window_handle, uint32 width, uint32 height)`
Creates a Vulkan swapchain for presenting rendered images.

**Parameters:**
- `window_handle`: Platform window handle (can be NULL for headless rendering)
- `width`: Swapchain width in pixels
- `height`: Swapchain height in pixels

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
if (!NxVulcan::create_swapchain(NULL, 1920, 1080)) {
    printf("Failed to create swapchain\n");
    return false;
}
```

**Note:** Current implementation provides headless rendering simulation without actual WSI (Window System Integration).

#### `void destroy_swapchain(void)`
Destroys the swapchain and associated resources (framebuffers, image views).

#### `bool recreate_swapchain(uint32 width, uint32 height)`
Recreates the swapchain with new dimensions (e.g., after window resize).

**Parameters:**
- `width`: New width in pixels
- `height`: New height in pixels

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
// Handle window resize
if (!NxVulcan::recreate_swapchain(1280, 720)) {
    printf("Failed to recreate swapchain\n");
}
```

#### `bool acquire_next_image(uint32 *image_index)`
Acquires the next available swapchain image for rendering.

**Parameters:**
- `image_index`: Output parameter receiving the acquired image index

**Returns:** `true` on success, `false` if swapchain needs recreation

**Example:**
```cpp
uint32 image_idx;
if (!NxVulcan::acquire_next_image(&image_idx)) {
    // Recreate swapchain
}
```

#### `bool present_image(uint32 image_index)`
Presents the rendered image to the screen.

**Parameters:**
- `image_index`: Index of image to present

**Returns:** `true` on success, `false` on failure

### 3. Render Pass and Pipeline

#### `bool create_render_pass(void)`
Creates a Vulkan render pass defining rendering operations.

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
if (!NxVulcan::create_render_pass()) {
    printf("Failed to create render pass\n");
    return false;
}
```

#### `void destroy_render_pass(void)`
Destroys the render pass.

#### `bool create_graphics_pipeline(void)`
Creates graphics pipeline and pipeline layout.

**Returns:** `true` on success, `false` on failure

**Note:** Full pipeline creation requires shader modules (not included in minimal implementation).

**Example:**
```cpp
if (!NxVulcan::create_graphics_pipeline()) {
    printf("Failed to create graphics pipeline\n");
    return false;
}
```

#### `void destroy_graphics_pipeline(void)`
Destroys the graphics pipeline and layout.

### 4. Command Buffer Management

#### `bool allocate_command_buffers(void)`
Allocates command buffers for recording rendering commands (one per swapchain image).

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
if (!NxVulcan::allocate_command_buffers()) {
    printf("Failed to allocate command buffers\n");
    return false;
}
```

#### `void free_command_buffers(void)`
Frees all allocated command buffers.

#### `bool begin_command_buffer(uint32 buffer_index)`
Begins recording commands into the specified command buffer.

**Parameters:**
- `buffer_index`: Index of command buffer to record into (0 to swapchain_image_count-1)

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
if (!NxVulcan::begin_command_buffer(0)) {
    printf("Failed to begin command buffer\n");
}
```

#### `bool end_command_buffer(uint32 buffer_index)`
Ends command buffer recording.

**Parameters:**
- `buffer_index`: Index of command buffer to finish recording

**Returns:** `true` on success, `false` on failure

#### `bool begin_render_pass(uint32 buffer_index, uint32 framebuffer_index)`
Begins a render pass within command buffer recording.

**Parameters:**
- `buffer_index`: Command buffer index
- `framebuffer_index`: Framebuffer index (typically matches image index)

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
NxVulcan::begin_command_buffer(0);
NxVulcan::begin_render_pass(0, 0);
// ... draw commands ...
NxVulcan::end_render_pass(0);
NxVulcan::end_command_buffer(0);
```

#### `void end_render_pass(uint32 buffer_index)`
Ends the current render pass.

**Parameters:**
- `buffer_index`: Command buffer index

### 5. Drawing Commands

#### `void bind_pipeline(uint32 buffer_index)`
Binds the graphics pipeline for subsequent draw calls.

**Parameters:**
- `buffer_index`: Command buffer index

#### `void bind_vertex_buffer(uint32 buffer_index, void *vertex_buffer)`
Binds a vertex buffer for drawing.

**Parameters:**
- `buffer_index`: Command buffer index
- `vertex_buffer`: Vulkan vertex buffer handle (from `upload_mesh_data`)

#### `void bind_index_buffer(uint32 buffer_index, void *index_buffer)`
Binds an index buffer for indexed drawing.

**Parameters:**
- `buffer_index`: Command buffer index
- `index_buffer`: Vulkan index buffer handle (from `upload_mesh_data`)

#### `void draw_indexed(uint32 buffer_index, uint32 index_count)`
Records an indexed draw call.

**Parameters:**
- `buffer_index`: Command buffer index
- `index_count`: Number of indices to draw

**Example:**
```cpp
// Inside render pass
NxVulcan::bind_pipeline(0);
NxVulcan::bind_vertex_buffer(0, mesh->pVulkanVertexBuffer);
NxVulcan::bind_index_buffer(0, mesh->pVulkanIndexBuffer);
NxVulcan::draw_indexed(0, mesh->m_num_indices);
```

#### `bool submit_command_buffer(uint32 buffer_index)`
Submits the recorded command buffer to the GPU for execution.

**Parameters:**
- `buffer_index`: Command buffer index

**Returns:** `true` on success, `false` on failure

**Example:**
```cpp
if (!NxVulcan::submit_command_buffer(0)) {
    printf("Failed to submit command buffer\n");
}
```

### 6. Mesh Management

#### `sMesh* create_mesh(uint32 checksum)`
Creates a new mesh object.

**Parameters:**
- `checksum`: Unique identifier for the mesh

**Returns:** Pointer to created mesh, or NULL on failure

#### `void destroy_mesh(sMesh *p_mesh)`
Destroys a mesh and frees associated resources.

#### `void upload_mesh_data(sMesh *p_mesh)`
Uploads mesh vertex and index data to GPU.

**Parameters:**
- `p_mesh`: Mesh with populated `mp_positions`, `mp_indices`, etc.

**Example:**
```cpp
sMesh *mesh = NxVulcan::create_mesh(0x12345678);
mesh->m_num_vertices = 3;
mesh->m_num_indices = 3;
mesh->mp_positions = new float[9];  // Fill with data
mesh->mp_indices = new uint16[3];   // Fill with data
NxVulcan::upload_mesh_data(mesh);
```

### 7. Scene Management

#### `sScene* create_scene(void)`
Creates a new scene object.

**Returns:** Pointer to created scene

#### `void destroy_scene(sScene *p_scene)`
Destroys a scene.

#### `void add_mesh_to_scene(sScene *p_scene, sMesh *p_mesh)`
Adds a mesh to the scene.

#### `void render_scene(sScene *p_scene, uint32 flags, uint32 viewport)`
Renders all meshes in the scene (high-level API).

**Parameters:**
- `p_scene`: Scene to render
- `flags`: Render flags (vRENDER_OPAQUE, vRENDER_SEMITRANSPARENT, etc.)
- `viewport`: Viewport index (default: 0)

## Complete Rendering Example

```cpp
#include "NX/render.h"

using namespace NxVulcan;

int main() {
    // 1. Initialize
    if (!init_vulkan()) return 1;
    if (!create_swapchain(NULL, 1920, 1080)) return 1;
    if (!create_render_pass()) return 1;
    if (!create_graphics_pipeline()) return 1;
    if (!allocate_command_buffers()) return 1;
    
    // 2. Create mesh
    sMesh *triangle = create_mesh(0x12345678);
    triangle->m_num_vertices = 3;
    triangle->m_num_indices = 3;
    triangle->mp_positions = new float[9] { /* vertex data */ };
    triangle->mp_indices = new uint16[3] {0, 1, 2};
    upload_mesh_data(triangle);
    
    // 3. Render loop
    for (int frame = 0; frame < 100; frame++) {
        uint32 img_idx;
        if (!acquire_next_image(&img_idx)) break;
        
        begin_command_buffer(img_idx);
        begin_render_pass(img_idx, img_idx);
        
        bind_pipeline(img_idx);
        bind_vertex_buffer(img_idx, triangle->pVulkanVertexBuffer);
        bind_index_buffer(img_idx, triangle->pVulkanIndexBuffer);
        draw_indexed(img_idx, triangle->m_num_indices);
        
        end_render_pass(img_idx);
        end_command_buffer(img_idx);
        
        submit_command_buffer(img_idx);
        present_image(img_idx);
    }
    
    // 4. Cleanup
    destroy_mesh(triangle);
    free_command_buffers();
    destroy_graphics_pipeline();
    destroy_render_pass();
    destroy_swapchain();
    shutdown_vulkan();
    
    return 0;
}
```

## Implementation Notes

### Current Limitations

1. **Headless Rendering**: The current implementation simulates swapchain without actual Window System Integration (WSI). Full WSI requires:
   - VK_KHR_surface extension
   - Platform-specific extensions (VK_KHR_xcb_surface, VK_KHR_win32_surface, etc.)
   - Window handle integration

2. **Shader Modules**: Pipeline creation is stubbed without actual shader compilation. Full implementation requires:
   - SPIR-V shader modules
   - Vertex input attribute descriptions
   - Complete pipeline state configuration

3. **Synchronization**: Basic synchronization is implemented. Production code should use:
   - Fences for CPU-GPU synchronization
   - Semaphores for GPU-GPU synchronization
   - Multiple frames in flight

4. **Descriptor Sets**: Not yet implemented. Required for:
   - Uniform buffers
   - Texture sampling
   - Push constants

### Future Enhancements

1. Complete WSI integration for actual window rendering
2. Shader module loading and compilation
3. Descriptor set management
4. Multiple render targets
5. Depth/stencil buffers
6. Multisampling (MSAA)
7. Compute pipelines

## See Also

- [VULKAN_RENDERER.md](../../../docs/VULKAN_RENDERER.md) - Overview
- [test_vulkan_integration.cpp](test_vulkan_integration.cpp) - Complete example
- [test_vulkan_rendering.cpp](test_vulkan_rendering.cpp) - Comprehensive test suite

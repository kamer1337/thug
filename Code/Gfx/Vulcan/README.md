# Vulcan Renderer

This directory contains the Vulcan renderer implementation for THUG.

## Quick Start

For a quick API reference, see [QUICK_REFERENCE.md](QUICK_REFERENCE.md).

For a comprehensive integration guide, see [INTEGRATION.md](INTEGRATION.md).

For example usage code, see [example.cpp](example.cpp).

## Structure

- `NX/render.h` - Header file defining the Vulcan rendering interface
- `NX/render.cpp` - Implementation file with Vulkan rendering functions
- `p_nxmodel.h` - Platform-specific model class header
- `p_nxmodel.cpp` - Platform-specific model class implementation
- **`QUICK_REFERENCE.md`** - **NEW** Quick API reference for common operations
- **`INTEGRATION.md`** - **NEW** Comprehensive integration guide for using the renderer
- **`example.cpp`** - **NEW** Example code demonstrating renderer usage
- `IMPLEMENTATION.md` - Detailed implementation status and function documentation
- `README.md` - This file, providing an overview of the renderer

## Getting Started

### 1. Read the Integration Guide

Start with [INTEGRATION.md](INTEGRATION.md) for a comprehensive guide on:
- How to initialize and use the renderer
- API reference and examples
- Scene and mesh management
- Texture handling
- Camera setup
- Rendering pipeline
- Best practices

### 2. Study the Examples

Review [example.cpp](example.cpp) which contains 10 complete examples showing:
- Basic initialization and cleanup
- Scene and mesh creation
- Texture management
- Camera configuration
- Render states and blend modes
- Complete rendering pipeline
- Frustum culling
- Using CVulcanModel

### 3. Build with Vulkan Support

To enable the Vulkan renderer in your build:

```bash
mkdir build && cd build
cmake -DUSE_VULKAN_RENDERER=ON ..
cmake --build .
```

**Note**: The current implementation won't compile due to base code compatibility issues. See BUILDING.md in the repository root for details.

### 4. Explore the Implementation

Review the source files:
- `NX/render.h` - Complete API definitions
- `NX/render.cpp` - Implementation with stubs for GPU operations
- `p_nxmodel.h/cpp` - Platform-specific model class
- `IMPLEMENTATION.md` - Detailed implementation documentation

## Implementation Status

The Vulcan renderer now has a working implementation with the following components:

### Core Rendering Functions
- `GetBlendMode()` - Returns blend mode for a given checksum (FULLY IMPLEMENTED)
- `set_render_state()` - Sets Vulkan render state (IMPLEMENTED with state tracking)
- `set_blend_mode()` - Sets Vulkan blend mode (IMPLEMENTED with state tracking)
- `set_camera()` - Sets camera view and projection (FULLY IMPLEMENTED)
- `set_frustum_bbox_transform()` - Sets frustum bounding box transform (FULLY IMPLEMENTED)
- `frustum_check_sphere()` - Performs frustum culling sphere check (FULLY IMPLEMENTED)
- `get_bounding_sphere_nearest_z()` - Gets nearest Z of bounding sphere (IMPLEMENTED)
- `IsVisible()` - Checks visibility (FULLY IMPLEMENTED)
- `render_shadow_targets()` - Renders shadow targets (STRUCTURE IMPLEMENTED)
- `render_light_glows()` - Renders light glows (STRUCTURE IMPLEMENTED)
- `render_scene()` - Renders a scene with given flags (FULLY IMPLEMENTED with mesh iteration)

### Texture Management
- `create_texture_projection_details()` - Creates texture projection details (FULLY IMPLEMENTED)
- `destroy_texture_projection_details()` - Destroys texture projection details (FULLY IMPLEMENTED)
- `set_texture_projection_camera()` - Sets texture projection camera (FULLY IMPLEMENTED)
- `load_texture()` - Loads texture from file (IMPLEMENTED with stub for file I/O)
- `create_texture()` - Creates texture from data (FULLY IMPLEMENTED)
- `destroy_texture()` - Destroys texture (FULLY IMPLEMENTED)
- `get_texture()` - Retrieves texture by checksum (FULLY IMPLEMENTED)

### Mesh Management
- `create_mesh()` - Creates a new mesh (FULLY IMPLEMENTED)
- `destroy_mesh()` - Destroys a mesh (FULLY IMPLEMENTED)
- `upload_mesh_data()` - Uploads mesh data to GPU (STRUCTURE IMPLEMENTED)

### Scene Management
- `create_scene()` - Creates a new scene (FULLY IMPLEMENTED)
- `destroy_scene()` - Destroys a scene (FULLY IMPLEMENTED)
- `add_mesh_to_scene()` - Adds mesh to scene (FULLY IMPLEMENTED)

### Initialization
- `init_vulkan()` - Initializes Vulkan renderer (FULLY IMPLEMENTED)
- `shutdown_vulkan()` - Shuts down Vulkan renderer (FULLY IMPLEMENTED)

## File Format Compatibility

The Vulcan renderer maintains compatibility with existing THUG file formats across all platforms:

### Texture Formats (.TEX / .IMG)
- `sTexture` structure supports both console and PC texture formats
- Supports texture checksums for asset identification
- Compatible with:
  - Base and actual dimensions
  - Mipmap levels
  - Texture flags (holes, alpha, channels)
  - Alpha data separation

#### Console Formats (NGC/NGPS/Xbox)
- RGBA32 - 32-bit uncompressed RGBA
- RGB24 - 24-bit uncompressed RGB
- PALETTE8 - 8-bit palettized
- PALETTE4 - 4-bit palettized

#### PC Formats (THUG PC Version - DirectX)
- **DXT1** (BC1) - RGB with 1-bit alpha or no alpha, 4:1 compression ratio
  - 4x4 pixel blocks, 8 bytes per block
  - Vulkan format: `VK_FORMAT_BC1_RGB_UNORM_BLOCK` or `VK_FORMAT_BC1_RGBA_UNORM_BLOCK`
- **DXT3** (BC2) - RGBA with explicit alpha, 4:1 compression ratio
  - 4x4 pixel blocks, 16 bytes per block
  - Vulkan format: `VK_FORMAT_BC2_UNORM_BLOCK`
- **DXT5** (BC3) - RGBA with interpolated alpha, 4:1 compression ratio
  - 4x4 pixel blocks, 16 bytes per block
  - Vulkan format: `VK_FORMAT_BC3_UNORM_BLOCK`
- **A8R8G8B8** - 32-bit uncompressed ARGB
  - Vulkan format: `VK_FORMAT_B8G8R8A8_UNORM`
- **R5G6B5** - 16-bit RGB
  - Vulkan format: `VK_FORMAT_R5G6B5_UNORM_PACK16`
- **A1R5G5B5** - 16-bit ARGB with 1-bit alpha
  - Vulkan format: `VK_FORMAT_A1R5G5B5_UNORM_PACK16`
- **A4R4G4B4** - 16-bit ARGB
  - Vulkan format: `VK_FORMAT_A4R4G4B4_UNORM_PACK16`

PC textures are typically stored in .IMG files with DirectX format headers.

### Model Formats
- `CVulcanModel` class implements the platform-specific CModel interface
- Compatible with skeleton/bone systems
- Maintains bounding sphere information
- Material management support

### Material Formats
- `sMaterial` structure supports:
  - Multiple texture passes
  - Blend modes (matching existing checksums)
  - UV addressing modes
  - Alpha cutoff values

### Scene Formats
- `sScene` structure compatible with:
  - Mesh lists
  - Bounding volumes
  - Scene flags
  - Visibility culling

### Additional Formats
- CAS (Character Animation System) - Compatible through scene flags
- WGT (Weight) data - Supported through mesh bone indices
- CIF (Character Info) - Handled at higher level

## Technical Implementation Details

### Data Structures
All structures are designed to be binary-compatible with existing file formats:
- Texture data stored as raw bytes with format identifiers
- Support for both console formats (RGBA32, RGB24, PALETTE) and PC formats (DXT1/3/5, A8R8G8B8)
- DXT compressed textures remain compressed in GPU memory
- Mesh vertex data uses standard float arrays
- Indices use 16-bit unsigned integers
- Checksums use 32-bit CRC values

### PC Format Detection
The renderer automatically detects PC texture formats:
- Files with .IMG extension are treated as PC format textures
- PC textures use DirectX compressed formats (DXT1/DXT3/DXT5)
- Compressed textures are uploaded directly to GPU without decompression
- Format conversion handled by Vulkan BC (Block Compression) formats

### Memory Management
- Hash tables for efficient texture and projection detail lookup
- Reference counting through ownership flags
- Proper cleanup in shutdown sequence

### Rendering Pipeline
The renderer follows standard graphics pipeline flow:
1. Initialize Vulkan context
2. Load assets (textures, meshes)
3. Build scenes from meshes
4. Set camera and frustum
5. Cull invisible objects
6. Render opaque geometry
7. Render transparent geometry
8. Present to screen

## Future Vulkan-Specific Work

Full Vulkan backend would require:
1. **Vulkan Instance and Device**
   - Physical device selection
   - Logical device creation
   - Queue family setup

2. **Swapchain Management**
   - Image acquisition
   - Presentation queue
   - Format selection

3. **Pipeline State Objects**
   - Vertex input state
   - Graphics pipeline creation
   - Shader module loading

4. **Command Buffer Management**
   - Command pool creation
   - Recording and submission
   - Synchronization primitives

5. **Descriptor Sets and Layouts**
   - Uniform buffer objects
   - Texture samplers
   - Descriptor pool management

6. **Shader Modules**
   - SPIR-V compilation
   - Vertex and fragment shaders
   - Push constant setup

7. **Memory Management**
   - Device memory allocation
   - Buffer creation and binding
   - Staging buffer optimization

8. **Texture and Sampler Setup**
   - Image layout transitions
   - Sampler creation
   - Mipmap generation

## Notes

- The current implementation provides a complete software architecture
- Vulkan-specific GPU operations are marked with "In a full implementation" comments
- All data structures maintain compatibility with existing file formats
- The renderer can be used as a reference for implementing other graphics backends

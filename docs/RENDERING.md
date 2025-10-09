# THUG Rendering Pipeline

## Overview

Tony Hawk's Underground (THUG) uses the Neversoft Engine's Nx rendering system, a flexible platform-independent graphics architecture. The rendering system supports multiple backends (DirectX, OpenGL, Vulkan) through a unified interface, managing scenes, meshes, textures, materials, and effects.

## Table of Contents

- [Architecture](#architecture)
- [Rendering Pipeline](#rendering-pipeline)
- [Scene Management](#scene-management)
- [Mesh and Geometry](#mesh-and-geometry)
- [Texture System](#texture-system)
- [Material System](#material-system)
- [Shader Usage](#shader-usage)
- [Platform Backends](#platform-backends)
- [Performance Optimization](#performance-optimization)

---

## Architecture

### System Structure

The Nx rendering system follows a layered architecture with platform abstraction:

```
┌─────────────────────────────────────────────────────┐
│           Game Layer (Sk/, Gel/)                    │
│    - Game objects, cameras, effects                 │
│    - High-level rendering requests                  │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│        Nx Platform-Independent Interface            │
│    - CEngine: Core rendering manager                │
│    - CScene: Scene graph management                 │
│    - CModel/CMesh: Geometry management              │
│    - CTexture: Texture management                   │
│    - CSector: Spatial partitioning                  │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│       Platform Abstraction Layer (p_nx*.h)          │
│    - Platform-specific implementations              │
└──────────────────┬──────────────────────────────────┘
                   ↓
      ┌────────────┴──────────┬──────────────┐
      ↓                       ↓              ↓
┌───────────┐         ┌──────────┐    ┌──────────┐
│  Vulkan   │         │ DirectX  │    │  OpenGL  │
│  Backend  │         │  Backend │    │  Backend │
└───────────┘         └──────────┘    └──────────┘
```

### Key Components

**File**: `Code/Gfx/nx.h` - Main rendering interface

1. **CEngine** - Central rendering manager
   - Scene loading and management
   - Render frame orchestration
   - Global rendering state

2. **CScene** - Scene graph container
   - Holds collection of sectors
   - Manages scene hierarchy
   - Collision data integration

3. **CSector** - Spatial partition unit
   - Groups related geometry
   - Enables culling and LOD
   - Named via checksums

4. **CModel/CMesh** - Geometry representation
   - Vertex and index buffers
   - Skeletal animation support
   - LOD levels

5. **CTexture** - Texture management
   - Loading from files
   - Texture dictionaries
   - VRAM management

6. **CTexDict** - Texture dictionary
   - Groups related textures
   - Shared between scenes
   - Reference counting

---

## Rendering Pipeline

### Frame Execution

The rendering pipeline executes each frame within the main game loop:

**File**: `Code/Gel/MainLoop/Mainloop.cpp`

```cpp
void Manager::MainLoop(void)
{
    while (!done)
    {
        // 1. Pre-render setup
        Nx::CEngine::sPreRender();
        
        // 2. Service systems (input, network, etc.)
        service_system();
        
        // 3. Update game logic
        game_logic();
        
        // 4. Render world geometry and objects
        Nx::CEngine::sRenderWorld();
        
        // 5. Post-render (present frame)
        Nx::CEngine::sPostRender();
        
        // 6. Frame timing
        Tmr::OncePerRender();
    }
}
```

### Render Phases

#### 1. Pre-Render Phase

```cpp
static void sPreRender()
```

**Responsibilities**:
- Clear frame buffers
- Set up render states
- Prepare double-buffered data
- Begin frame timing

#### 2. Render World Phase

```cpp
static void sRenderWorld()
```

**Responsibilities**:
- Frustum culling
- Render opaque geometry
- Render transparent geometry (sorted)
- Apply post-processing effects
- Render UI overlays
- Render debug visualization

**Rendering Order**:
1. Sky/background
2. Opaque geometry (front-to-back for early-Z)
3. Transparent geometry (back-to-front for blending)
4. Particles and effects
5. 2D UI elements
6. Debug overlays

#### 3. Post-Render Phase

```cpp
static void sPostRender()
```

**Responsibilities**:
- Finalize rendering commands
- Present frame buffer to display
- Swap buffers
- Collect frame statistics

### Frame Timing

**File**: `Code/Sys/Timer.cpp`

The engine targets specific frame rates:
- **Console**: 60 FPS (16.67ms per frame)
- **PC**: Configurable (30/60/uncapped)

Frame timing uses `Tmr::OncePerRender()` to maintain consistent update rates.

---

## Scene Management

### CScene Class

**File**: `Code/Gfx/NxScene.h`

The `CScene` class manages a collection of sectors that make up a level:

```cpp
class CScene
{
public:
    CScene(int sector_table_size = 10);
    virtual ~CScene();
    
    // Loading
    bool LoadTextures(const char *p_name);
    bool LoadCollision(const char *p_name);
    
    // Sector management
    CSector* GetSector(uint32 sector_checksum);
    void AddSector(CSector *pSector);
    CSector* CreateSector();
    bool DeleteSector(uint32 sector_checksum);
    
    // Scene manipulation
    uint32 CloneSector(uint32 orig_sector_checksum);
    void UpdateSuperSectors();
};
```

### Scene Loading

Scenes are loaded from platform-specific files:

```cpp
CScene* CEngine::sLoadScene(
    const char *p_name,           // Scene name
    CTexDict *p_tex_dict,         // Texture dictionary
    bool add_super_sectors = true, // Enable spatial optimization
    bool is_sky = false,          // Sky scene flag
    bool is_dictionary = false,   // Dictionary scene flag
    bool is_net = false           // Network scene flag
)
```

**Loading Process**:
1. **Parse scene file** (platform-specific format)
2. **Create sectors** from scene data
3. **Load textures** into texture dictionary
4. **Build collision** data structures
5. **Initialize super sectors** for spatial queries
6. **Link to render graph**

### Scene Hierarchy

Scenes organize geometry hierarchically:

```
Scene (Level)
  ├── Sector (Building)
  │     ├── Mesh (Walls)
  │     ├── Mesh (Floor)
  │     └── Mesh (Roof)
  ├── Sector (Street)
  │     ├── Mesh (Road)
  │     └── Mesh (Sidewalk)
  └── Sector (Props)
        ├── Model (Bench)
        ├── Model (Tree)
        └── Model (Lamppost)
```

### Super Sectors

**File**: `Code/Sk/Engine/supersector.h`

Super sectors provide spatial partitioning for efficient culling:

```cpp
class SSec::Manager
{
    // Query for visibility
    void GetIntersectingCells(Mth::CBBox &bbox, 
                              Lst::Head<CellEntry> *p_list);
    
    // Update spatial structure
    void UpdateCellInfo();
};
```

**Benefits**:
- **Frustum culling**: Only render visible sectors
- **Occlusion culling**: Skip hidden geometry
- **LOD selection**: Choose detail level by distance
- **Collision queries**: Fast spatial lookups

---

## Mesh and Geometry

### CMesh Class

**File**: `Code/Gfx/NxMesh.h`

Meshes represent renderable geometry:

```cpp
class CMesh
{
public:
    CMesh();
    virtual ~CMesh();
    
    // Rendering
    void Render();
    void SetActive(bool active);
    
    // Transformation
    void SetPos(const Mth::Vector &pos);
    void SetOrientation(const Mth::Matrix &mat);
    void SetScale(const Mth::Vector &scale);
    
    // Material
    void SetMaterial(uint32 material_checksum);
    void SetColor(uint32 rgba);
    void SetBlendMode(uint32 blend);
    
    // Visibility
    void SetVisible(bool visible);
    bool IsVisible() const;
};
```

### Geometry Data

Meshes contain:

**Vertex Data**:
- Position (x, y, z)
- Normal (nx, ny, nz)
- Texture coordinates (u, v)
- Vertex colors (rgba)
- Bone weights (skinning)
- Bone indices (skinning)

**Index Data**:
- Triangle lists
- Triangle strips (optimized)

**Vertex Buffers**:
```cpp
struct sMesh
{
    void* pVertices;        // Vertex buffer
    uint16* pIndices;       // Index buffer
    uint32 numVertices;     // Vertex count
    uint32 numIndices;      // Index count
    uint32 vertexStride;    // Bytes per vertex
};
```

### CModel Class

**File**: `Code/Gfx/NxModel.h`

Models are collections of meshes with hierarchical structure:

```cpp
class CModel
{
public:
    CModel();
    virtual ~CModel();
    
    // Loading
    bool Load(const char *p_filename);
    
    // Rendering
    void Render();
    void SetVisibility(uint32 mask);
    
    // Hierarchy
    CHierarchy* GetHierarchy();
    int GetNumBones() const;
    
    // Animation
    void SetPose(float *p_matrices);
    void ApplyAnimation(CAnimData *p_anim, float time);
};
```

### Skeletal Animation

Models support skeletal animation:

**File**: `Code/Gfx/BonedAnim.h`

1. **Hierarchy**: Bone parent-child relationships
2. **Bind Pose**: Initial bone transformations
3. **Animation Data**: Keyframe transforms over time
4. **Skinning**: Vertex deformation by bones

**Skinning Process**:
```
For each vertex:
    finalPos = 0
    For each bone weight:
        boneTransform = boneMatrix[boneIndex]
        finalPos += weight * (boneTransform * vertexPos)
```

---

## Texture System

### CTexture Class

**File**: `Code/Gfx/NxTexture.h`

Textures represent 2D image data for materials:

```cpp
class CTexture
{
public:
    CTexture();
    virtual ~CTexture();
    
    // Loading
    bool LoadTexture(const char *p_texture_name, 
                     bool sprite, 
                     bool alloc_vram = true);
    
    bool LoadTextureFromBuffer(uint8* p_buffer, 
                               int buffer_size,
                               uint32 texture_checksum,
                               bool sprite, 
                               bool alloc_vram = true);
    
    // Manipulation
    bool Generate32BitImage(bool renderable = false);
    bool CombineTextures(CTexture *p_texture);
    bool AdjustBrightness(float brightness_scale);
    
    // Properties
    uint32 GetChecksum() const;
    uint16 GetWidth() const;
    uint16 GetHeight() const;
    uint8 GetBitDepth() const;
};
```

### Texture Formats

THUG supports multiple texture formats:

**Console Formats**:
- **TEX**: PlayStation 2 native format
- **XBX**: Xbox native format
- **NGC**: GameCube native format

**PC Formats**:
- **IMG**: Custom image format
- **PNG**: Portable Network Graphics (conversion)
- **TGA**: Targa (development)

### Texture Dictionaries

**File**: `Code/Gfx/NxTexMan.h`

Texture dictionaries group related textures:

```cpp
class CTexDict
{
public:
    CTexDict(uint32 checksum);
    
    // Texture management
    CTexture* GetTexture(uint32 texture_checksum);
    bool LoadTexture(const char *p_texture_name);
    bool AddTexture(CTexture *p_texture);
    
    // Dictionary management
    void LoadFromFile(const char *p_filename);
    void AddRef();
    void Release();
};
```

**Benefits**:
- **Sharing**: Multiple models use same textures
- **Memory efficiency**: Single copy in VRAM
- **Loading**: Batch texture loading
- **Reference counting**: Automatic cleanup

### Texture Streaming

Textures can be loaded/unloaded dynamically:

```cpp
// Load texture into VRAM
bool CTexture::AddToVram()

// Unload texture from VRAM
bool CTexture::RemoveFromVram()
```

This enables:
- **Reduced memory usage**: Stream on demand
- **Level transitions**: Swap texture sets
- **LOD textures**: Load detail based on distance

---

## Material System

### Material Properties

Materials define surface appearance:

**Properties**:
- **Diffuse color**: Base surface color
- **Specular**: Shininess and highlights
- **Emissive**: Self-illumination
- **Transparency**: Alpha blending
- **Texture maps**: Diffuse, normal, specular

### Blend Modes

**File**: `Code/Gfx/nx.h`

Materials support various blend modes:

```cpp
enum EBlendMode
{
    vBLEND_MODE_OPAQUE,         // No blending (opaque)
    vBLEND_MODE_BLEND,          // Alpha blending
    vBLEND_MODE_ADD,            // Additive (fire, glow)
    vBLEND_MODE_SUBTRACT,       // Subtractive
    vBLEND_MODE_MODULATE,       // Multiply (shadow)
    vBLEND_MODE_BRIGHTEN,       // Screen blend
    vBLEND_MODE_DIFFUSE,        // Diffuse only
};
```

**Blend Equations**:

- **Blend**: `finalColor = srcColor * srcAlpha + dstColor * (1 - srcAlpha)`
- **Add**: `finalColor = srcColor + dstColor`
- **Modulate**: `finalColor = srcColor * dstColor`

### Material Assignment

Materials are assigned to meshes:

```cpp
// Set material by checksum
pMesh->SetMaterial(CRCD(0x12345678, "MaterialName"));

// Override color
pMesh->SetColor(0xFFAABBCC);  // RGBA

// Set blend mode
pMesh->SetBlendMode(vBLEND_MODE_BLEND);
```

---

## Shader Usage

### Shader System

THUG's rendering originally used fixed-function pipeline, but modern backends support shaders:

**Shader Types**:
1. **Vertex Shader**: Transform vertices, compute lighting
2. **Fragment/Pixel Shader**: Compute per-pixel colors

### Common Shaders

**Basic Textured**:
```glsl
// Vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(position, 1.0);
    fragTexCoord = texCoord;
}

// Fragment
in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D diffuseTexture;

void main()
{
    fragColor = texture(diffuseTexture, fragTexCoord);
}
```

**Skinned Mesh**:
```glsl
// Vertex
uniform mat4 boneMatrices[64];

layout(location = 4) in vec4 boneIndices;
layout(location = 5) in vec4 boneWeights;

void main()
{
    mat4 skinMatrix = 
        boneMatrices[int(boneIndices.x)] * boneWeights.x +
        boneMatrices[int(boneIndices.y)] * boneWeights.y +
        boneMatrices[int(boneIndices.z)] * boneWeights.z +
        boneMatrices[int(boneIndices.w)] * boneWeights.w;
    
    vec4 skinnedPos = skinMatrix * vec4(position, 1.0);
    gl_Position = mvpMatrix * skinnedPos;
}
```

---

## Platform Backends

### Backend Architecture

The Nx system abstracts platform-specific rendering:

**Platform Headers**: `Code/Gfx/*/p_nx*.h`

Each backend implements:
- Device management
- Texture loading/management
- Mesh rendering
- Shader/material system
- Scene graph traversal

### Vulkan Backend ✅

**Status**: Complete (software architecture)
**Location**: `Code/Gfx/Vulcan/`

**Features**:
- Modern low-level API
- Explicit control over GPU
- Excellent multi-threading
- Cross-platform (Windows, Linux, macOS via MoltenVK)

**Implementation**: See `Code/Gfx/Vulcan/IMPLEMENTATION.md`

### DirectX Backend 🔨

**Status**: Stub implementation
**Location**: `Code/Gfx/DirectX/`

**Target API**: DirectX 8/9
**Platform**: Windows only

**Required**:
- DirectX SDK
- D3D device management
- Texture/mesh upload to GPU
- Shader compilation (if using shaders)

**References**: See `Code/Gfx/DirectX/README.md`

### OpenGL Backend 🔨

**Status**: Stub implementation
**Location**: `Code/Gfx/OpenGL/`

**Target API**: OpenGL 3.3+
**Platform**: Windows, Linux, macOS

**Required**:
- OpenGL context creation (GLFW, SDL2)
- VBO/VAO for geometry
- GLSL shader compilation
- Texture upload (glTexImage2D)

---

## Performance Optimization

### Culling

**Frustum Culling**: Skip objects outside camera view

```cpp
bool IsInFrustum(const Mth::CBBox &bbox, 
                 const Gfx::Camera &camera);
```

**Occlusion Culling**: Skip objects hidden behind others
- Portal-based culling (indoor)
- Potential visibility sets (outdoor)

### Level of Detail (LOD)

Models have multiple detail levels:

```cpp
enum ELodLevel
{
    LOD_HIGH,    // Close to camera
    LOD_MEDIUM,  // Medium distance
    LOD_LOW,     // Far from camera
};

// Select LOD based on distance
int SelectLOD(float distance)
{
    if (distance < 20.0f) return LOD_HIGH;
    if (distance < 50.0f) return LOD_MEDIUM;
    return LOD_LOW;
}
```

### Batching

**Static Batching**: Combine static geometry
- Reduces draw calls
- Improves GPU efficiency

**Dynamic Batching**: Combine similar objects
- Same material/texture
- Small vertex counts

### Memory Management

**Texture Compression**: Reduce VRAM usage
- DXT/BC formats
- Platform-specific compression

**Geometry Compression**: Reduce vertex size
- Quantized positions/normals
- Packed vertex colors

**Streaming**: Load/unload on demand
- Texture streaming
- Geometry streaming
- Audio streaming

### Profiling

**Frame Time Breakdown**:
```
Total:      16.67ms (60 FPS)
  Update:    5.00ms (game logic)
  Render:   10.00ms (rendering)
    Culling:  1.00ms
    Opaque:   6.00ms
    Trans:    2.00ms
    UI:       1.00ms
  Other:     1.67ms
```

**GPU Profiling**: Measure render phases
- Setup time
- Draw call time
- Shader time
- Texture bandwidth

---

## Related Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall engine architecture
- [GAME_LOOP.md](GAME_LOOP.md) - Frame timing and execution
- [BACKEND_ARCHITECTURE.md](BACKEND_ARCHITECTURE.md) - Graphics backend details
- [docs/CODE_STYLE.md](CODE_STYLE.md) - Coding conventions

## Implementation Files

### Core Rendering
- `Code/Gfx/nx.h/cpp` - Main rendering interface
- `Code/Gfx/NxScene.h/cpp` - Scene management
- `Code/Gfx/NxSector.h/cpp` - Sector management
- `Code/Gfx/NxModel.h/cpp` - Model/mesh system
- `Code/Gfx/NxMesh.h/cpp` - Mesh geometry
- `Code/Gfx/NxTexture.h/cpp` - Texture system
- `Code/Gfx/NxTexMan.h/cpp` - Texture manager
- `Code/Gfx/gfxman.h/cpp` - Graphics manager

### Animation
- `Code/Gfx/BonedAnim.h/cpp` - Skeletal animation
- `Code/Gfx/AnimController.h/cpp` - Animation control
- `Code/Gfx/Skeleton.h/cpp` - Skeleton hierarchy

### Camera
- `Code/Gfx/camera.h/cpp` - Camera system
- `Code/Gfx/NxViewMan.h/cpp` - Viewport management
- `Code/Gfx/NxViewport.h/cpp` - Viewport definition

### Effects
- `Code/Gfx/NxNewParticle.h/cpp` - Particle system
- `Code/Gfx/NxWeather.h/cpp` - Weather effects
- `Code/Gfx/shadow.h/cpp` - Shadow rendering

### Platform Backends
- `Code/Gfx/Vulcan/` - Vulkan backend (complete)
- `Code/Gfx/DirectX/` - DirectX backend (stub)
- `Code/Gfx/OpenGL/` - OpenGL backend (stub)

---

*This document describes the rendering pipeline and graphics architecture of the THUG engine.*

# THUG Asset File Formats

This document describes the file formats used by Tony Hawk's Underground for storing game assets. Understanding these formats is essential for asset extraction, conversion, and creation.

## Overview

THUG uses custom binary formats optimized for PlayStation 2, GameCube, Xbox, and PC. The PC version maintains compatibility while using platform-appropriate storage.

## Table of Contents

1. [Model Files (.mdl)](#model-files-mdl)
2. [Texture Files (.tex, .img)](#texture-files-tex-img)
3. [Scene Files (.scn)](#scene-files-scn)
4. [Animation Files (.ska)](#animation-files-ska)
5. [Skeleton Files (.ske)](#skeleton-files-ske)
6. [Collision Files (.col)](#collision-files-col)
7. [Script Files (.qb)](#script-files-qb)
8. [Audio Files](#audio-files)

---

## Model Files (.mdl)

### Format Overview
- **Extension**: `.mdl`
- **Type**: Binary
- **Platform**: PC (DirectX-compatible)

### File Structure

```
Header (32 bytes)
├── Magic Number: "NMDL" (4 bytes)
├── Version: uint32 (4 bytes)
├── Mesh Count: uint32 (4 bytes)
├── Material Count: uint32 (4 bytes)
├── Vertex Count: uint32 (4 bytes)
├── Index Count: uint32 (4 bytes)
├── Bone Count: uint32 (4 bytes) [if skinned]
└── Flags: uint32 (4 bytes)

Mesh Data (per mesh)
├── Mesh Header
│   ├── Name: char[64]
│   ├── Material Index: uint32
│   ├── Vertex Offset: uint32
│   ├── Vertex Count: uint32
│   ├── Index Offset: uint32
│   └── Index Count: uint32
├── Vertices
│   ├── Position: float[3] (x, y, z)
│   ├── Normal: float[3]
│   ├── UV: float[2]
│   ├── Color: uint32 (RGBA)
│   └── Bone Weights: float[4] [if skinned]
└── Indices
    └── uint16[] or uint32[]

Material Data (per material)
├── Name: char[64]
├── Texture Path: char[256]
├── Shader Type: uint32
├── Color: float[4] (RGBA)
├── Specular: float
├── Glossiness: float
└── Flags: uint32

Bounding Data
├── AABB Min: float[3]
├── AABB Max: float[3]
└── Bounding Sphere: float[4] (center + radius)
```

### Loading Example

```cpp
// Code/Gfx/NxModel.cpp
CModel* LoadModel(const char* filename)
{
    File::CFile* pFile = File::Open(filename, File::mREAD);
    
    // Read header
    ModelHeader header;
    pFile->Read(&header, sizeof(ModelHeader));
    
    // Validate magic number
    if (header.magic != 'NMDL')
        return nullptr;
    
    // Read mesh data...
    // Read material data...
    // Create DirectX buffers...
    
    return pModel;
}
```

### Supported Features
- ✅ Static meshes
- ✅ Skinned meshes (up to 4 bone weights per vertex)
- ✅ Multiple materials per model
- ✅ LOD levels
- ✅ Vertex colors
- ✅ Multiple UV channels
- ❌ Morph targets (not used in THUG)

---

## Texture Files (.tex, .img)

### Format Overview
- **Extension**: `.tex` (header), `.img` (data)
- **Type**: Binary
- **Compression**: DXT1, DXT3, DXT5, or uncompressed

### File Structure (.tex)

```
Header
├── Magic Number: "NTEX" (4 bytes)
├── Version: uint32
├── Width: uint16
├── Height: uint16
├── Mipmap Count: uint8
├── Format: uint8
│   ├── 0 = Uncompressed RGBA8888
│   ├── 1 = DXT1 (no alpha)
│   ├── 2 = DXT3 (sharp alpha)
│   └── 3 = DXT5 (smooth alpha)
├── Flags: uint32
└── Data Offset: uint32

Mipmap Data (per level)
├── Width: uint16
├── Height: uint16
├── Data Size: uint32
└── Data Offset: uint32
```

### Compression Details

#### DXT1
- Compression: 6:1
- Alpha: 1-bit (on/off)
- Best for: Solid textures, environment

#### DXT3
- Compression: 4:1
- Alpha: 4-bit (16 levels)
- Best for: UI elements with transparency

#### DXT5
- Compression: 4:1
- Alpha: Interpolated 8-bit
- Best for: Character textures, smooth gradients

### Texture Loading

```cpp
// Code/Gfx/NxTexture.cpp
CTexture* LoadTexture(const char* filename)
{
    // Read .tex header
    TextureHeader header = ReadTextureHeader(filename);
    
    // Read .img data
    void* pImageData = ReadImageData(filename);
    
    // Create DirectX texture
    IDirect3DTexture9* pD3DTexture;
    D3DXCreateTextureFromFileInMemory(
        pDevice, pImageData, header.dataSize, &pD3DTexture
    );
    
    return new CTexture(pD3DTexture, header);
}
```

---

## Scene Files (.scn)

### Format Overview
- **Extension**: `.scn`
- **Type**: Binary
- **Purpose**: Level geometry and object placement

### File Structure

```
Header
├── Magic: "NSCN" (4 bytes)
├── Version: uint32
├── Object Count: uint32
├── Light Count: uint32
├── Sector Count: uint32
└── Script Reference: uint32

Object Data (per object)
├── Type: uint32 (static, dynamic, trigger)
├── Name: char[64]
├── Model Reference: char[256]
├── Position: float[3]
├── Rotation: float[4] (quaternion)
├── Scale: float[3]
└── Properties: variant data

Sector Data (per sector)
├── Bounds: AABB
├── Object List: uint32[]
├── Visibility: bitfield
└── Collision Reference: uint32

Light Data (per light)
├── Type: uint32 (point, spot, directional)
├── Position: float[3]
├── Color: float[3] (RGB)
├── Intensity: float
├── Range: float
└── Direction: float[3] [if directional/spot]
```

### Scene Loading

```cpp
// Code/Sk/Engine/supersector.cpp
bool LoadScene(const char* filename)
{
    SceneHeader header = ReadSceneHeader(filename);
    
    // Load all referenced models
    for (uint32 i = 0; i < header.objectCount; i++)
    {
        ObjectData obj = ReadObject();
        LoadModel(obj.modelReference);
        PlaceObject(obj);
    }
    
    // Setup sectors for occlusion
    SetupSectors(header);
    
    return true;
}
```

---

## Animation Files (.ska)

### Format Overview
- **Extension**: `.ska`
- **Type**: Binary
- **Compression**: Keyframe reduction and quantization

### File Structure

```
Header
├── Magic: "NSKA" (4 bytes)
├── Version: uint32
├── Duration: float (seconds)
├── Frame Rate: float (fps)
├── Bone Count: uint32
├── Channel Count: uint32
└── Flags: uint32

Bone Data (per bone)
├── Name: char[64]
├── Parent Index: sint32
├── Bind Pose: Matrix4x4
└── Inverse Bind: Matrix4x4

Channel Data (per channel)
├── Bone Index: uint32
├── Channel Type: uint8
│   ├── 0 = Translation
│   ├── 1 = Rotation
│   └── 2 = Scale
├── Keyframe Count: uint32
└── Keyframes
    ├── Time: float
    └── Value: float[3] or float[4] (quaternion)
```

### Animation Compression

THUG uses several compression techniques:
- **Keyframe Reduction**: Remove redundant keyframes
- **Quaternion Compression**: Store as 3 components, derive 4th
- **Delta Encoding**: Store differences between frames
- **Quantization**: Reduce precision for minor movements

### Animation Loading

```cpp
// Code/Gfx/Skeleton.cpp
CAnimation* LoadAnimation(const char* filename)
{
    AnimHeader header = ReadAnimHeader(filename);
    
    CAnimation* pAnim = new CAnimation();
    pAnim->SetDuration(header.duration);
    pAnim->SetFrameRate(header.frameRate);
    
    // Load channels
    for (uint32 i = 0; i < header.channelCount; i++)
    {
        AnimChannel channel = ReadChannel();
        pAnim->AddChannel(channel);
    }
    
    return pAnim;
}
```

---

## Skeleton Files (.ske)

### Format Overview
- **Extension**: `.ske`
- **Type**: Binary
- **Purpose**: Skeletal rig definition

### File Structure

```
Header
├── Magic: "NSKE" (4 bytes)
├── Version: uint32
├── Bone Count: uint32
└── Root Bone Index: uint32

Bone Data (per bone)
├── Name: char[64]
├── Parent Index: sint32 (-1 for root)
├── Local Position: float[3]
├── Local Rotation: float[4] (quaternion)
├── Local Scale: float[3]
├── Bind Matrix: Matrix4x4
└── Inverse Bind Matrix: Matrix4x4
```

---

## Collision Files (.col)

### Format Overview
- **Extension**: `.col`
- **Type**: Binary
- **Purpose**: Collision geometry and surface properties

### File Structure

```
Header
├── Magic: "NCOL" (4 bytes)
├── Version: uint32
├── Triangle Count: uint32
├── Vertex Count: uint32
└── Surface Count: uint32

Vertex Data
└── Position: float[3]

Triangle Data (per triangle)
├── Vertex Indices: uint32[3]
├── Normal: float[3]
├── Surface Index: uint32
└── Flags: uint32
    ├── Grindable
    ├── Wallrideable
    ├── Non-collidable
    └── Trigger

Surface Data (per surface)
├── Name: char[64]
├── Friction: float
├── Elasticity: float
├── Sound Type: uint32
└── Flags: uint32
```

### Collision Flags

```cpp
// Code/Gel/Collision/CollEnums.h
enum CollisionFlags
{
    mSD_NON_COLLIDABLE   = (1 << 0),
    mSD_GRINDABLE        = (1 << 1),
    mSD_WALLRIDEABLE     = (1 << 2),
    mSD_TRIGGER          = (1 << 3),
    mSD_VERT             = (1 << 4),
    mSD_WATER            = (1 << 5),
    mSD_INVISIBLE        = (1 << 6),
    // ... more flags
};
```

---

## Script Files (.qb)

### Format Overview
- **Extension**: `.qb` (compiled), `.q` (source)
- **Type**: Binary (QB bytecode)
- **Purpose**: Game logic and level scripts

### Compiled Format (.qb)

```
Header
├── Magic: "NQBC" (4 bytes)
├── Version: uint32
├── Script Count: uint32
├── Symbol Table Size: uint32
└── Code Size: uint32

Symbol Table
└── (name_checksum, offset) pairs

Script Data (per script)
├── Name Checksum: uint32
├── Parameter Count: uint32
├── Local Count: uint32
├── Code Offset: uint32
└── Code Size: uint32

Bytecode
└── QB instructions
```

### Script Commands

See `Code/Gel/Scripting/` for the QB script system implementation.

---

## Audio Files

### Music
- **Format**: `.ogg` (Vorbis) or `.wav`
- **Sample Rate**: 44.1 kHz
- **Channels**: Stereo
- **Bitrate**: 128-192 kbps for OGG

### Sound Effects
- **Format**: `.wav`
- **Sample Rate**: 22.05 kHz or 44.1 kHz
- **Channels**: Mono or Stereo
- **Bit Depth**: 16-bit

### Streaming Audio
- **Format**: `.wav` or `.ogg`
- **Used for**: Cutscene audio, ambient sounds
- **Loaded**: Asynchronously via `Code/Sys/File/AsyncFilesys.cpp`

---

## Asset Conversion

### From Original Console Formats

The original THUG assets need conversion from console formats:

1. **PS2 → PC**: Convert texture formats, adjust shaders
2. **GameCube → PC**: Convert texture swizzling
3. **Xbox → PC**: Usually compatible, minor adjustments

### Tools Needed

- Texture converter (DXT compression)
- Model exporter (to .mdl format)
- Animation converter
- Script compiler (QB → .qb)

See `tools/` directory for asset conversion utilities.

---

## Platform-Specific Notes

### Windows (DirectX)
- Native format support
- Hardware texture compression
- Fast loading via memory mapping

### Linux/Mac (OpenGL)
- Format conversion at load time
- Software texture decompression if needed
- May need texture format translation

---

## See Also

- `Data/README.md` - Asset directory structure
- `Code/Gel/AssMan/` - Asset Manager implementation
- `Code/Sys/File/` - File I/O system
- `tools/` - Asset extraction and conversion tools

---

**Technical Reference**: This documentation is based on reverse engineering and analysis of the THUG source code. File format specifications may vary slightly between game versions.

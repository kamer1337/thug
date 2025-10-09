# Graphics and Audio Backend Architecture

This document describes the graphics and audio backend architecture for THUG PC port.

## Overview

THUG PC port supports multiple rendering and audio backends through a platform abstraction layer. This allows the game to run on different platforms with different graphics APIs and audio systems.

## Graphics Backends

### Supported Backends

1. **Vulkan** ✅ (Implemented)
   - Location: `Code/Gfx/Vulcan/`
   - Status: Complete software architecture, ready for Vulkan API integration
   - Platforms: Windows, Linux, macOS (via MoltenVK)
   - Features: Modern low-level API, explicit control, excellent performance

2. **DirectX** 🔨 (Stub)
   - Location: `Code/Gfx/DirectX/`
   - Status: Directory structure and documentation provided
   - Platforms: Windows only
   - Features: Native Windows API, good driver support

3. **OpenGL** 🔨 (Stub)
   - Location: `Code/Gfx/OpenGL/`
   - Status: Directory structure and documentation provided
   - Platforms: Windows, Linux, macOS
   - Features: Cross-platform, mature, widely supported

### Architecture

```
Nx Rendering System (Platform-Independent)
    ├── NxScene - Scene management
    ├── NxModel - Model/mesh management
    ├── NxTexture - Texture management
    └── Platform Abstraction Layer
        ├── Vulkan Backend (p_nx*.h in Vulcan/)
        ├── DirectX Backend (p_nx*.h in DirectX/)
        └── OpenGL Backend (p_nx*.h in OpenGL/)
```

### Implementation Status

#### Vulkan Backend ✅
- **Complete**: All 28 rendering functions implemented
- **Data Structures**: sTexture, sMesh, sMaterial, sScene
- **Features**:
  - Texture loading (console TEX, PC IMG formats)
  - Mesh management with VBO support
  - Scene hierarchy and culling
  - Material/blend mode system
  - Camera and projection
  - File format compatibility

See `Code/Gfx/Vulcan/IMPLEMENTATION.md` for detailed status.

#### DirectX Backend 🔨
- **Status**: Stub implementation
- **Required**: DirectX 8/9 SDK
- **Functions**: Device management, texture loading, mesh rendering
- **Priority**: High (for Windows-only deployment)

#### OpenGL Backend 🔨
- **Status**: Stub implementation
- **Required**: OpenGL 3.3+, GLEW/GLAD
- **Functions**: Context management, VBO/VAO, GLSL shaders
- **Priority**: Medium (for cross-platform support)

### Choosing a Backend

**For new projects:**
- Use **Vulkan** if targeting modern hardware and want best performance
- Use **OpenGL** if targeting older hardware or need broadest compatibility
- Use **DirectX** if targeting Windows only with legacy support

**For this project:**
- Vulkan backend is most complete and recommended
- DirectX backend recommended for Windows-specific features
- OpenGL backend good for maximum compatibility

## Audio Backends

### Supported Backends

1. **SDL2_mixer** 🔨 (Recommended)
   - Location: `Code/Gel/Music/SDL2/`
   - Status: Stub implementation with documentation
   - Platforms: Windows, Linux, macOS, and more
   - Features: Simple API, multiple formats, free

2. **OpenAL** 🔨 (Alternative)
   - Location: `Code/Gel/Music/OpenAL/`
   - Status: Stub implementation with documentation
   - Platforms: Windows, Linux, macOS
   - Features: 3D positional audio, hardware acceleration, free

3. **FMOD** 🔨 (Professional)
   - Location: `Code/Gel/Music/FMOD/`
   - Status: Stub implementation with documentation
   - Platforms: All major platforms
   - Features: Professional grade, advanced features, requires license

### Architecture

```
Pcm Audio System (Platform-Independent)
    ├── Music playback
    ├── Sound effects
    ├── Volume control
    └── Platform Abstraction Layer
        ├── SDL2_mixer Backend
        ├── OpenAL Backend
        └── FMOD Backend
```

### Implementation Status

All audio backends are currently stub implementations. The interfaces are defined but require actual library integration.

#### Current Stubs (Win32)
- Location: `Code/Gel/Music/Win32/p_music.h`
- All functions return default/empty values
- Need to be replaced with actual backend calls

### Choosing an Audio Backend

**Recommended: SDL2_mixer**
- Free and open source
- Simple to integrate
- Cross-platform
- Good format support
- Active development

**For 3D audio: OpenAL**
- Excellent positional audio
- Hardware acceleration
- Industry standard
- Free and open source

**For professional features: FMOD**
- AAA game quality
- Advanced DSP and mixing
- Excellent documentation
- Requires commercial license

## Implementation Guide

### Graphics Backend Implementation

1. **Choose a backend** (Vulkan recommended, already implemented)
2. **Implement device management** (initialization, context, cleanup)
3. **Implement texture loading** (from file, format conversion)
4. **Implement mesh rendering** (vertex buffers, draw calls)
5. **Implement scene management** (camera, frustum culling)
6. **Test with existing game code**

See `Code/Gfx/Vulcan/INTEGRATION.md` for detailed integration guide.

### Audio Backend Implementation

1. **Choose a backend** (SDL2_mixer recommended for simplicity)
2. **Initialize audio system** (device, context, channels)
3. **Implement music streaming** (load, play, pause, stop)
4. **Implement sound effects** (load, play, volume)
5. **Implement volume control** (master, music, effects)
6. **Test with game audio**

See audio backend README files for implementation examples.

## Build System Integration

### CMake Configuration

The CMake build system supports multiple backends:

```cmake
# Vulkan support (optional)
option(USE_VULKAN_RENDERER "Use Vulkan renderer" OFF)

# DirectX support (Windows only)
if(WIN32)
    option(USE_DIRECTX_RENDERER "Use DirectX renderer" ON)
endif()

# OpenGL support
option(USE_OPENGL_RENDERER "Use OpenGL renderer" OFF)

# Audio backend selection
set(AUDIO_BACKEND "SDL2" CACHE STRING "Audio backend (SDL2/OpenAL/FMOD)")
```

### Platform-Specific Builds

**Windows (DirectX recommended):**
```bash
cmake -DUSE_DIRECTX_RENDERER=ON -DAUDIO_BACKEND=SDL2 ..
```

**Linux (OpenGL or Vulkan):**
```bash
cmake -DUSE_VULKAN_RENDERER=ON -DAUDIO_BACKEND=SDL2 ..
```

**Cross-platform (OpenGL):**
```bash
cmake -DUSE_OPENGL_RENDERER=ON -DAUDIO_BACKEND=SDL2 ..
```

## File Organization

```
Code/
├── Gfx/                          # Graphics
│   ├── Vulcan/                   # Vulkan backend ✅
│   │   ├── NX/
│   │   │   ├── render.h
│   │   │   └── render.cpp
│   │   ├── p_nxmodel.h
│   │   ├── p_nxmodel.cpp
│   │   ├── IMPLEMENTATION.md
│   │   ├── INTEGRATION.md
│   │   └── README.md
│   ├── DirectX/                  # DirectX backend 🔨
│   │   └── README.md
│   ├── OpenGL/                   # OpenGL backend 🔨
│   │   └── README.md
│   └── Win32/
│       └── p_memview.h
└── Gel/
    └── Music/                    # Audio
        ├── Win32/                # Current stubs
        │   └── p_music.h
        ├── SDL2/                 # SDL2_mixer backend 🔨
        │   └── README.md
        ├── OpenAL/               # OpenAL backend 🔨
        │   └── README.md
        └── FMOD/                 # FMOD backend 🔨
            └── README.md
```

Legend:
- ✅ Implemented
- 🔨 Stub/Documentation only

## Next Steps

### High Priority
1. ✅ Fix C++ template syntax issues (COMPLETED)
2. ✅ Document graphics backend architecture (COMPLETED)
3. ✅ Document audio backend architecture (COMPLETED)
4. 🔨 Implement DirectX backend (or use Vulkan)
5. 🔨 Implement audio backend (SDL2_mixer recommended)

### Medium Priority
1. Implement OpenGL backend for maximum compatibility
2. Test Vulkan backend integration
3. Add audio streaming support
4. Implement 3D positional audio

### Low Priority
1. Optimize rendering pipeline
2. Add advanced audio effects
3. Support additional audio formats
4. Hardware acceleration optimization

## References

### Graphics APIs
- [Vulkan Tutorial](https://vulkan-tutorial.com/)
- [OpenGL Tutorial](https://learnopengl.com/)
- [DirectX 9 Documentation](https://docs.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics)

### Audio APIs
- [SDL2_mixer Documentation](https://www.libsdl.org/projects/SDL_mixer/docs/)
- [OpenAL Documentation](https://www.openal.org/documentation/)
- [FMOD Documentation](https://www.fmod.com/docs/)

### Project Documentation
- `docs/platforms/STUB_FUNCTIONS.md` - Complete function list
- `docs/platforms/WIN32.md` - Win32 platform specifics
- `Code/Gfx/Vulcan/IMPLEMENTATION.md` - Vulkan implementation details

## Contributing

See `docs/CONTRIBUTING.md` for guidelines on implementing these backends.

---

**Status**: Template syntax fixed ✅, Architecture documented ✅, Backends need implementation 🔨

# OpenGL Graphics Backend

This directory contains stub implementations for OpenGL 3.3+ graphics backend for THUG PC port.

## Status

**STUB IMPLEMENTATION** - These files provide the interface structure but require actual OpenGL API implementation.

## Purpose

The OpenGL backend provides:
- OpenGL context management
- Texture loading and management
- Mesh/geometry rendering with VBOs/VAOs
- Scene management
- Shader program management (GLSL)

## Implementation Requirements

To complete this backend, you will need:

1. **OpenGL** - OpenGL 3.3 or higher
2. **GLEW** or **GLAD** - Extension loading library
3. **Context Creation** - GLFW, SDL2, or platform-specific (WGL/GLX)

## Files

- `p_nxdevice.h` - Device/context management interface
- `p_nxdevice.cpp` - Device implementation (stub)
- `p_nxtexture.h` - Texture management interface
- `p_nxtexture.cpp` - Texture implementation (stub)
- `p_nxmesh.h` - Mesh/VBO interface
- `p_nxmesh.cpp` - Mesh implementation (stub)
- `p_nxshader.h` - GLSL shader interface
- `p_nxshader.cpp` - Shader implementation (stub)

## Integration

The OpenGL backend integrates with the existing Nx rendering system through platform-specific headers (p_nx*.h).

## Advantages

- **Cross-platform** - Works on Windows, Linux, macOS
- **Open standard** - Well-documented and widely supported
- **Modern features** - Programmable pipeline with GLSL shaders

## Alternative Backends

- **Vulkan** - Modern low-level API (already implemented in `Code/Gfx/Vulcan/`)
- **DirectX** - Windows-only API (see `Code/Gfx/DirectX/`)

## References

- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [Learn OpenGL](https://learnopengl.com/)
- Vulkan implementation: `Code/Gfx/Vulcan/`
- See `docs/platforms/STUB_FUNCTIONS.md` for detailed function list

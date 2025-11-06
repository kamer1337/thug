# Win32 Platform Documentation

## Overview

This document describes the Win32 (Windows 32-bit/64-bit) platform implementation for Tony Hawk's Underground. The original game supported Windows PC alongside console platforms (PS2, Xbox, GameCube). This repository now focuses exclusively on the PC port, with all console-specific code removed.

## Current State

### What's Implemented

✅ **Keyboard Input System** - Complete implementation with comprehensive key mapping
- Keyboard controls fully mapped to game controller buttons
- See [docs/PC_KEYBOARD_CONTROLS.md](../PC_KEYBOARD_CONTROLS.md) for details

✅ **Build System** - CMake-based configuration
- Modern build system for cross-platform PC development
- Supports Windows, Linux, and macOS build targets

✅ **File System** - Win32 file I/O abstractions
- Basic file operations implemented
- Directory traversal and path handling

### What's Not Implemented (Stubs)

The following Win32 subsystems contain stub functions that need implementation:

❌ **Graphics/Rendering** - Most D3D8 functions are stubs
❌ **Audio System** - DirectSound/XAudio2 implementations needed
❌ **Window Management** - Partial implementation, needs completion
❌ **Threading** - Win32 threading primitives need implementation
❌ **Networking** - Winsock-based networking incomplete

See [STUB_FUNCTIONS.md](STUB_FUNCTIONS.md) for detailed list of stub functions.

## Platform Requirements

### Development Requirements

#### Windows
- **OS**: Windows 7 SP1 or later (Windows 10/11 recommended)
- **Compiler**: Microsoft Visual C++ 2015 or later (MSVC 14.0+)
  - Visual Studio 2019 or 2022 recommended
- **SDK**: Windows SDK 10.0 or later
- **Build Tool**: CMake 3.10 or later

#### Linux (Cross-compilation or Wine)
- **Compiler**: GCC 7+ or Clang 6+
- **MinGW**: For Windows PE compilation
- **Wine**: For testing Windows executables on Linux
- **Build Tool**: CMake 3.10 or later

#### macOS (Cross-compilation)
- **Compiler**: Clang 6+ (via Xcode or Command Line Tools)
- **MinGW**: For Windows PE cross-compilation
- **Build Tool**: CMake 3.10 or later

### Runtime Requirements

#### Minimum Specifications
- **OS**: Windows 7 SP1 (32-bit or 64-bit)
- **CPU**: Pentium 4 or equivalent (SSE2 support)
- **RAM**: 512 MB minimum, 1 GB recommended
- **Graphics**: Vulcan compatible graphics card
- **Sound**: DirectSound compatible audio device
- **Storage**: 2 GB available space (for game + assets)

#### Recommended Specifications
- **OS**: Windows 10 (64-bit) or later
- **CPU**: Modern multi-core processor
- **RAM**: 4 GB or more
- **Graphics**: Modern DirectX 11+ compatible GPU
- **Sound**: Modern audio device
- **Storage**: SSD with 5 GB free space

## Platform-Specific Code Locations

### Core Win32 Code

```
Code/
├── Sys/
│   └── Win32/              # Windows-specific system code
│       ├── p_display.cpp   # Display/window management
│       ├── p_input.cpp     # Keyboard/mouse input (IMPLEMENTED)
│       ├── p_audio.cpp     # Audio system (STUBS)
│       ├── p_file.cpp      # File I/O
│       └── p_thread.cpp    # Threading (STUBS)
└── Gfx/
    └── DX8/                # Vulcan graphics implementation
        ├── p_nxdevice.cpp  # D3D device management (STUBS)
        ├── p_nxscene.cpp   # Scene rendering (STUBS)
        ├── p_nxtexture.cpp # Texture management (STUBS)
        └── p_nxmesh.cpp    # Mesh rendering (STUBS)
```

### Platform Abstraction Layer

The codebase uses a platform abstraction system with:
- Platform-specific files prefixed with `p_` (e.g., `p_input.cpp`)
- Platform-independent interfaces in shared headers
- Conditional compilation using preprocessor directives

Example pattern:
```cpp
#if defined(__PLAT_WN32__)
    // Windows-specific implementation
    #include "Sys/Win32/p_input.h"
#elif defined(__PLAT_LINUX__)
    // Linux implementation
    #include "Sys/Linux/p_input.h"
#endif
```

## Win32 API Usage

### Primary APIs Used

#### Windows Core APIs
- **Kernel32.dll**: File I/O, memory management, threading
- **User32.dll**: Window creation, message handling, input
- **GDI32.dll**: Basic graphics operations (minimal usage)

#### Vulcan Graphics
- **D3D8.dll**: Direct3D 8 rendering
- **D3DX8.dll**: Helper utilities for D3D8

#### Audio
- **DirectSound (dsound.dll)**: Legacy audio system
- May need migration to XAudio2 for modern Windows

#### Networking
- **Winsock2 (ws2_32.dll)**: Network communications
- **WinHTTP**: HTTP communications for online features

### Key Windows Structures

```cpp
// Window handle
HWND g_hWnd;

// DirectX device
LPDIRECT3DDEVICE8 g_pD3DDevice;

// DirectSound device
LPDIRECTSOUND8 g_pDirectSound;
```

## Build Configuration

### CMake Platform Detection

The build system detects Windows platform:

```cmake
if(WIN32)
    add_definitions(-D__PLAT_WN32__)
    # Windows-specific compiler flags
    if(MSVC)
        add_compile_options(/W3 /MP)
    endif()
endif()
```

### Preprocessor Definitions

Key platform defines:
- `__PLAT_WN32__` - Windows platform marker
- `_WIN32` - Standard Windows define
- `WIN32_LEAN_AND_MEAN` - Minimal Windows headers
- `NOMINMAX` - Avoid min/max macro conflicts

### Compiler Flags

#### MSVC (Visual Studio)
```
/W3           # Warning level 3
/MP           # Multi-processor compilation
/EHsc         # Exception handling
/std:c++14    # C++14 standard (or later)
/D_CRT_SECURE_NO_WARNINGS  # Disable CRT security warnings
```

#### GCC/Clang (MinGW)
```
-Wall         # Enable warnings
-std=c++14    # C++14 standard
-mwindows     # Windows subsystem
-municode     # Unicode support
```

## Graphics Implementation

### Vulcan Architecture

The game was originally designed for Vulcan:

```cpp
// D3D8 initialization (stub in current code)
HRESULT InitD3D(HWND hWnd)
{
    // Create D3D object
    LPDIRECT3D8 pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    
    // Set up present parameters
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    
    // Create device
    pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                       D3DCREATE_HARDWARE_VERTEXPROCESSING,
                       &d3dpp, &g_pD3DDevice);
    
    return S_OK;
}
```

### Modern Graphics Options

For a modern port, consider:

1. **Keep D3D8** - Use Vulcan for historical accuracy
   - Requires D3D8 SDK or redistribution DLLs
   - May have compatibility issues on modern Windows

2. **Upgrade to D3D11/12** - Modern DirectX versions
   - Better performance and features
   - Wide compatibility on Windows 7+
   - Requires shader rewriting

3. **OpenGL** - Cross-platform graphics
   - Works on Windows, Linux, macOS
   - Requires abstraction layer changes
   - Good for cross-platform ports

4. **Vulkan** - Modern low-level API
   - Excellent performance
   - Cross-platform support
   - Higher implementation complexity

## Audio Implementation

### DirectSound (Legacy)

Original implementation uses DirectSound:

```cpp
// DirectSound initialization
HRESULT InitDirectSound(HWND hWnd)
{
    DirectSoundCreate8(NULL, &g_pDirectSound, NULL);
    g_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
    return S_OK;
}
```

### Modern Audio Options

1. **XAudio2** - Modern Windows audio
   - Recommended for Windows 7+
   - Built into Windows 8+
   - Low latency, good performance

2. **OpenAL** - Cross-platform audio
   - Works on Windows, Linux, macOS
   - Good abstraction layer
   - Industry standard for games

3. **SDL2 Audio** - Simple cross-platform option
   - Easy to use
   - Good for basic audio needs
   - Works everywhere

## Input Implementation

### Keyboard Input (Implemented)

The keyboard input system is fully implemented:

```cpp
// Key mappings from keyboard to controller
static const KeyMapping g_keyMappings[] = {
    { VK_UP,    CONTROLLER_BUTTON_UP },
    { VK_DOWN,  CONTROLLER_BUTTON_DOWN },
    { VK_LEFT,  CONTROLLER_BUTTON_LEFT },
    { VK_RIGHT, CONTROLLER_BUTTON_RIGHT },
    // ... more mappings
};
```

See [docs/PC_KEYBOARD_CONTROLS.md](../PC_KEYBOARD_CONTROLS.md) for complete key mapping details.

### Mouse Input (Partial)

Basic mouse input is implemented but not fully integrated:
- Mouse movement detection
- Button press detection
- Needs integration with camera/menu systems

### Game Controller Input (Not Implemented)

DirectInput or XInput support would be needed for game controllers:

```cpp
// Example XInput integration (not implemented)
#include <xinput.h>

XINPUT_STATE state;
ZeroMemory(&state, sizeof(XINPUT_STATE));
XInputGetState(0, &state);  // Get controller 0 state
```

## Threading and Synchronization

### Current State

Win32 threading primitives are mostly stubs:
- Thread creation
- Mutexes and critical sections
- Semaphores
- Events

### Implementation Needed

```cpp
// Example Win32 thread creation
HANDLE hThread = CreateThread(
    NULL,                   // Security attributes
    0,                      // Stack size (0 = default)
    ThreadFunction,         // Thread function
    pParam,                 // Parameter to thread function
    0,                      // Creation flags
    &dwThreadId            // Thread identifier
);
```

## Networking

### Current State

Winsock2 networking is partially implemented:
- Socket creation
- Basic TCP/IP communications
- Needs completion for multiplayer

### Implementation Notes

```cpp
// Winsock initialization
WSADATA wsaData;
WSAStartup(MAKEWORD(2,2), &wsaData);

// Socket creation
SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
```

## Known Issues and Limitations

### Current Limitations

1. **Graphics Stubs** - Most rendering functions are not implemented
2. **Audio Stubs** - Sound system needs implementation
3. **Missing Assets** - Game data files required but not included
4. **Platform Dependencies** - Some original Win32 APIs may not work on modern Windows
5. **Vulcan** - Legacy API, may need upgrading

### Compatibility Issues

1. **Windows 10/11** - Vulcan may have compatibility issues
2. **64-bit Windows** - Some 32-bit code may need updating
3. **Modern Compilers** - Some old C++ patterns may generate warnings
4. **API Deprecation** - Some Win32 APIs are deprecated or removed

## Development Guidelines

### Adding Win32 Code

When implementing Win32 features:

1. **Use Platform Abstraction** - Add to existing `p_*.cpp` files
2. **Error Handling** - Always check Win32 API return values
3. **Unicode** - Use Unicode (wide) strings where appropriate
4. **Modern APIs** - Prefer modern equivalents when available
5. **Cross-Platform** - Keep portability in mind

### Example Pattern

```cpp
// In p_file.cpp (Win32 implementation)
#include <windows.h>

namespace File
{
    bool Open(const char* filename)
    {
        HANDLE hFile = CreateFileA(
            filename,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        
        if (hFile == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            // Handle error
            return false;
        }
        
        // Success
        return true;
    }
}
```

### Testing on Windows

1. **Build Configuration** - Use CMake to generate Visual Studio project
2. **Debug Build** - Use MSVC debugger for development
3. **Release Build** - Test optimized builds separately
4. **Windows Versions** - Test on Windows 7, 10, and 11
5. **32-bit vs 64-bit** - Test both architectures

## Migration Paths

### Option 1: Complete Original Win32 Implementation

**Goal**: Implement all stubs with original Win32 APIs
**Pros**: 
- Historical accuracy
- Matches original game
**Cons**:
- Vulcan compatibility issues
- Outdated APIs
- Limited to Windows

### Option 2: Modernized Win32 Port

**Goal**: Use modern Windows APIs (D3D11, XAudio2, etc.)
**Pros**:
- Better compatibility
- Modern features
- Better performance
**Cons**:
- Requires significant refactoring
- Still Windows-only

### Option 3: Cross-Platform Abstraction

**Goal**: Abstract platform layer, support multiple platforms
**Pros**:
- Works on Windows, Linux, macOS
- Modern APIs (SDL2, OpenGL/Vulkan, OpenAL)
- Future-proof
**Cons**:
- Most complex
- Requires major architectural changes

## Resources

### Official Documentation

- [Windows API Reference](https://docs.microsoft.com/en-us/windows/win32/api/)
- [DirectX Documentation](https://docs.microsoft.com/en-us/windows/win32/directx)
- [Visual Studio Documentation](https://docs.microsoft.com/en-us/visualstudio/)

### Community Resources

- Windows Game Programming forums
- DirectX development communities
- Game engine development resources

### Related Documentation

- [../BUILDING.md](../../BUILDING.md) - Build instructions
- [../PC_KEYBOARD_CONTROLS.md](../PC_KEYBOARD_CONTROLS.md) - Keyboard controls
- [STUB_FUNCTIONS.md](STUB_FUNCTIONS.md) - List of stub functions
- [../../ROADMAP.md](../../ROADMAP.md) - Development roadmap

## Contributing

When contributing Win32 platform code:

1. Review [../../docs/CONTRIBUTING.md](../CONTRIBUTING.md) for general guidelines
2. Follow [../../docs/CODE_STYLE.md](../CODE_STYLE.md) for code style
3. Test on multiple Windows versions
4. Document any new APIs or patterns used
5. Update this document with implementation progress

---

**Status**: Documentation in progress. Win32 implementation partially complete.
**Last Updated**: 2024-10
**Maintainers**: Community contributors

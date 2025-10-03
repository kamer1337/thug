# Building THUG - Technical Details

This document provides technical details about building Tony Hawk's Underground source code.

## Current Status

The repository has been prepared with a modern build system, but **the code cannot be compiled as-is** on modern systems. This document explains why and what would be needed.

## Build Infrastructure Added

### 1. CMake Build System

A `CMakeLists.txt` has been created that:
- Configures for multiple platforms (Linux, macOS, Windows)
- Collects source files automatically
- Excludes platform-specific console code
- Sets up include directories
- Provides debug/release build configurations

### 2. Case-Sensitivity Fixes

The original code was developed on Windows (case-insensitive filesystem). On Linux/macOS:
- Created symbolic links: `core -> Core`, `gel -> Gel`, etc.
- Created lowercase symlinks for all header files
- This allows `#include <core/defines.h>` to find `Core/Defines.h`

### 3. Path Separator Fixes

- Converted Windows backslash paths (`sys\mem\region.h`) to Unix forward slashes (`sys/mem/region.h`) in include statements
- Only fixed include directives, preserved line continuation backslashes

## Why It Won't Compile

### 1. Compiler Incompatibilities

**Variadic Macro Extensions**:
```cpp
#define Dbg_Printf(A...)  { printf(##A); }  // GCC requires __VA_ARGS__
```

The code uses MSVC/CodeWarrior specific variadic macro syntax that isn't ISO C++ standard.

**Fix Required**: Convert to standard `__VA_ARGS__` syntax:
```cpp
#define Dbg_Printf(...)  { printf(__VA_ARGS__); }
```

### 2. Missing Type Definitions

The code uses platform-specific types not defined for modern platforms:
```cpp
sint32, sint64, uint32, uint64  // Platform-specific integer types
```

**Fix Required**: Define these types or use standard `<cstdint>` types.

### 3. Platform-Specific Code

#### PlayStation 2 Specific:
- VU1 microcode (Vector Units)
- GS (Graphics Synthesizer) registers
- DMA controller code
- EE (Emotion Engine) specific features

#### GameCube Specific:
- Gekko CPU features
- DSP audio code
- GX graphics library calls

#### Xbox Specific:
- NV2A GPU code
- Xbox SDK calls

### 4. Missing Dependencies

#### Required Libraries (Not Included):
- **Sound**: `libsn.a`, background music system
- **Graphics**: `libgraph.a`, `libdma.a`, `libvu0.a`
- **Platform**: `libdev.a`, `libpad.a`, `libpkt.a`
- **Middleware**: Video codecs, audio systems
- **Game Engine**: Asset loading, script execution

#### Game Assets (Not Included):
- 3D Models (.mdl files)
- Textures (.tex files)
- Scripts (.qb files)
- Audio (music, sound effects)
- Animations
- Level data

## What Would Be Needed for a Full Build

### Approach 1: Original Platform Build

Requirements:
1. **PlayStation 2 SDK** (SN Systems ProDG or Official Sony SDK)
2. **ee-gcc** toolchain
3. **dvpasm** (VU assembler)
4. PS2 libraries (libsn, libgraph, libdma, etc.)
5. Game assets
6. Link file (`app.cmd`)

### Approach 2: Modern PC Port

This would require:
1. **Abstract Platform Layer**: Create a HAL (Hardware Abstraction Layer)
2. **Replace Console APIs**: 
   - Graphics: Port to OpenGL/DirectX/Vulkan
   - Audio: Port to OpenAL/FMOD
   - Input: Port to SDL2
3. **Fix Compiler Issues**: Convert to ISO C++
4. **Type Definitions**: Create platform-independent types
5. **Asset Pipeline**: Convert console assets to PC formats
6. **Rebuild Game Engine**: Reimplement missing middleware

### Approach 3: Documentation/Reference Only

- Use the code as reference for understanding game mechanics
- Study algorithms and architecture
- Document the codebase structure
- Learn from the implementation

## Attempting a Build

### Step 1: Configure

```bash
cd /path/to/thug
mkdir build && cd build
cmake ..
```

This will succeed - CMake can configure the build.

### Step 2: Build

```bash
cmake --build .
```

This will fail with errors like:
- "pasting '(' and '"string"' does not give a valid preprocessing token"
- "'sint32' does not name a type"
- Missing platform headers

### Step 3: Understanding the Errors

The errors are expected and indicate fundamental incompatibilities, not simple bugs.

## Files Modified/Added

### New Files:
- `CMakeLists.txt` - Modern build configuration
- `README.md` - Project documentation
- `.gitignore` - Build artifact exclusions
- `BUILDING.md` - This file

### Modified Files:
- Include paths in headers/source files (backslash to forward slash)

### Created Symlinks:
- `Code/core` -> `Code/Core`
- `Code/gel` -> `Code/Gel`
- `Code/gfx` -> `Code/Gfx`
- `Code/sk` -> `Code/Sk`
- `Code/sys` -> `Code/Sys`
- Subdirectory symlinks (e.g., `assman` -> `AssMan`)
- Header file symlinks (e.g., `defines.h` -> `Defines.h`)

## For Developers

### If You Want to Port This Code:

1. **Start Small**: Pick one subsystem (e.g., math library)
2. **Abstract Platform Code**: Create interfaces for platform-specific features
3. **Use Modern C++**: Update to C++11/14/17 standards
4. **Modern Libraries**: Replace proprietary middleware
5. **Test Incrementally**: Build and test each component

### Recommended Approach:

```
1. Core utilities and math (no platform dependencies)
2. Memory management (abstract allocators)
3. File I/O (abstract with C++ streams or modern APIs)
4. Graphics (abstract with modern API)
5. Game logic (dependent on above layers)
```

### Resources Needed:

- **Documentation**: Original platform SDK docs
- **Assets**: Game files from retail copy
- **Reference**: Similar open-source game engines
- **Time**: This is a multi-year effort

## Legal Note

This is proprietary source code. Any porting or modification work must respect:
- Intellectual property rights
- Original licenses
- Trademark protections
- Copyright law

This code is provided for **educational and preservation purposes only**.

## Conclusion

While the build system is in place, actually compiling this code requires either:
1. The original console development environment
2. Extensive porting work to modern platforms
3. Using it as reference documentation only

The CMake build system serves as a foundation for anyone brave enough to attempt a port, but be aware it's a significant undertaking requiring deep knowledge of:
- Console hardware architecture
- Graphics programming
- Game engine design
- C++ and low-level programming
- Cross-platform development

Good luck! 🛹

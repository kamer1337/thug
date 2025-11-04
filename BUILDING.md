# Building THUG - Technical Details (PC Port)

This document provides technical details about building Tony Hawk's Underground source code for PC.

## Current Status

This repository has been cleaned to focus on PC-only development. All console-specific code (PlayStation 2, GameCube, Xbox) has been removed. Significant progress has been made on PC compatibility:

**✅ Recently Fixed:**
- Type definitions for Linux/macOS platforms (sint32, uint32, sint64, uint64)
- Variadic macro syntax issues (changed to inline functions for PC)
- Pointer casting for 64-bit compatibility
- Added stub implementations for sound, music, and movie systems
- **C++ template syntax compatibility with modern GCC** (November 2024)

**❌ Still Blocking:**
- Missing graphics/rendering implementations
- Missing game assets
- Additional platform-specific stubs needed

This document explains the current state and what would be needed for a full build.

## Recent PC Port Progress (2024)

### What Was Fixed

1. **Platform Type Definitions** - Added proper type definitions for Linux and macOS
2. **Variadic Macro Compatibility** - Changed debug macros to use inline functions instead of non-standard variadic syntax
3. **64-bit Pointer Casting** - Fixed pointer arithmetic to work correctly on 64-bit systems
4. **Audio/Video Stub Implementations** - Added proper stub .cpp files for:
   - Sound effects system (`p_sfx.cpp`)
   - Movie playback (`p_movies.cpp`)
   - Music streaming (`p_music.cpp`)
5. **C++ Template Compatibility** (November 2024) - Fixed template member access issues in `Lst::Head<_T>` class:
   - Added `typename` qualifier for dependent types (e.g., `typename Node<_T>::Priority`)
   - Added `this->` prefix for inherited member access in templates
   - Added `Node<_T>::` qualifier for base class constants
   - Fixed platform-specific build issues (excluded Win32 files on Linux/macOS)

### What Still Needs Work

1. **Graphics Implementation** - Backend architecture documented and integrated into CMake, stub implementations exist for DirectX/OpenGL, Vulkan implementation exists but needs integration
2. **Complete Audio Implementation** - Backend architecture documented and integrated into CMake, stub implementations exist for SDL2/OpenAL/FMOD
3. **Asset Pipeline** - Convert console assets to PC formats
4. **Additional Platform Stubs** - Some platform-specific code still needs stub implementations

## Build Infrastructure Added

### 1. CMake Build System

A `CMakeLists.txt` has been created that:
- Configures for PC platforms (Linux, macOS, Windows)
- Collects source files automatically from Win32/Wn32 directories
- Sets up include directories
- Provides debug/release build configurations
- **NEW**: Supports multiple graphics backends (Vulkan, DirectX, OpenGL)
- **NEW**: Supports multiple audio backends (SDL2, OpenAL, FMOD)

#### Backend Architecture

The build system now supports configurable graphics and audio backends:

**Graphics Backend Options:**
```bash
cmake -DUSE_VULKAN_RENDERER=ON ..   # Vulkan (recommended, cross-platform)
cmake -DUSE_DIRECTX_RENDERER=ON ..  # DirectX (Windows only)
cmake -DUSE_OPENGL_RENDERER=ON ..   # OpenGL (cross-platform)
```

**Audio Backend Options:**
```bash
cmake -DAUDIO_BACKEND=SDL2 ..       # SDL2_mixer (recommended, simple API)
cmake -DAUDIO_BACKEND=OpenAL ..     # OpenAL (3D positional audio)
cmake -DAUDIO_BACKEND=FMOD ..       # FMOD (professional, requires license)
```

**Combined Example:**
```bash
cmake -DUSE_VULKAN_RENDERER=ON -DAUDIO_BACKEND=SDL2 ..
```

See `docs/BACKEND_ARCHITECTURE.md` for detailed backend documentation.

### 2. Case-Sensitivity Fixes

The original code was developed on Windows (case-insensitive filesystem). On Linux/macOS:
- Created symbolic links: `core -> Core`, `gel -> Gel`, etc.
- Created lowercase symlinks for all header files
- This allows `#include <core/defines.h>` to find `Core/Defines.h`

### 3. Path Separator Fixes

- Converted Windows backslash paths (`sys\mem\region.h`) to Unix forward slashes (`sys/mem/region.h`) in include statements
- Only fixed include directives, preserved line continuation backslashes

## Why It Won't Compile

### 1. Compiler Incompatibilities (PARTIALLY FIXED)

**Variadic Macro Extensions** (FIXED):
```cpp
#define Dbg_Printf(A...)  { printf(##A); }  // Old non-standard syntax
```

The code originally used MSVC/CodeWarrior specific variadic macro syntax that isn't ISO C++ standard.

**Fix Applied**: Changed PC platforms to use inline functions instead of variadic macros:
```cpp
inline void Dbg_Printf( const char* A ... ) {};  // Now works on Linux/Mac
```

### 2. Missing Type Definitions (FIXED)

The code uses platform-specific types that were not defined for Linux/Mac platforms:
```cpp
sint32, sint64, uint32, uint64  // Platform-specific integer types
```

**Fix Applied**: Added type definitions for Linux and macOS:
```cpp
#if defined(__PLAT_LINUX__) || defined(__PLAT_MACOS__)
typedef int                 int32;
typedef unsigned int        uint32;
typedef signed int          sint32;
typedef long long           int64;
typedef unsigned long long  uint64;
typedef signed long long    sint64;
#endif
```

### 3. Incomplete Win32 Implementation (PARTIALLY IMPROVED)

The Win32/Wn32 directories contain many stub functions that were never fully implemented:
- Graphics rendering functions (return NULL or empty implementations) - **STILL NEEDS WORK**
- Sound system functions - **NOW HAVE STUB IMPLEMENTATIONS**
- Movie playback - **NOW HAVE STUB IMPLEMENTATIONS**  
- Music streaming - **NOW HAVE STUB IMPLEMENTATIONS**
- Memory management (basic implementations)

**Recent Progress**: Added proper stub implementation files:
- `Code/Gel/SoundFX/Win32/p_sfx.cpp` - Sound effects stub implementations
- `Code/Gel/Movies/Win32/p_movies.cpp` - Movie playback stub implementations
- `Code/Gel/Music/Win32/p_music.cpp` - Music/streaming stub implementations

These stubs allow the code to link properly even though the functionality is not yet implemented.

### 4. C++ Template Compatibility Issues (FIXED)

The code used older C++ template syntax that was incompatible with modern GCC. This has been fixed in November 2024:

**Issues Fixed:**
- Template-dependent type names now use `typename` qualifier
- Inherited template members accessed via `this->` prefix
- Base class constants qualified with `Node<_T>::` syntax
- Platform-specific includes for template-using files

**Example of fixes applied:**
```cpp
// Before (doesn't compile on modern GCC):
Priority new_pri = node->GetPri();
node_init();
GetNext();

// After (compatible with modern GCC):
typename Node<_T>::Priority new_pri = node->GetPri();
this->node_init();
this->GetNext();
```

**Status**: ✅ Template compatibility issues in `Code/core/list/head.h` have been resolved. The code now compiles past template errors on modern GCC.

### 5. Missing Dependencies

#### Required Libraries (Not Included):
- **Sound**: Background music system, audio middleware
- **Graphics**: Modern rendering backend (OpenGL/DirectX/Vulkan)
- **Platform**: Input handling, file I/O
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

### Approach 1: Complete Win32 Implementation

Requirements:
1. **Complete Win32 stub functions** - Implement all the empty/stub functions in Win32 directories
2. **Modern rendering backend** - Port graphics to OpenGL/DirectX/Vulkan
3. **Audio system** - Implement complete audio/music system (OpenAL, FMOD, etc.)
4. **Input handling** - Complete controller and keyboard input
5. **Game assets** - Convert console assets to PC formats
6. **Fix compiler issues** - Convert to ISO C++

### Approach 2: Modern PC Port with Modern Libraries

This would require:
1. **Abstract Platform Layer**: Create a HAL (Hardware Abstraction Layer)
2. **Replace Win32 Stubs**: 
   - Graphics: Implement with OpenGL/DirectX/Vulkan
   - Audio: Implement with OpenAL/FMOD/SDL_mixer
   - Input: Implement with SDL2 or DirectInput
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

**Basic configuration (no backends):**
```bash
cd /path/to/thug
mkdir build && cd build
cmake ..
```

**With graphics backend:**
```bash
cmake -DUSE_VULKAN_RENDERER=ON ..
# or
cmake -DUSE_OPENGL_RENDERER=ON ..
```

**With audio backend:**
```bash
cmake -DAUDIO_BACKEND=SDL2 ..
```

**With both:**
```bash
cmake -DUSE_VULKAN_RENDERER=ON -DAUDIO_BACKEND=SDL2 ..
```

This will succeed - CMake can configure the build and will display a summary showing selected backends.

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

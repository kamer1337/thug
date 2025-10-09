# Building THUG - Technical Details (PC Port)

This document provides technical details about building Tony Hawk's Underground source code for PC.

## Current Status

This repository has been cleaned to focus on PC-only development. All console-specific code (PlayStation 2, GameCube, Xbox) has been removed. The code still **cannot be compiled as-is** on modern systems due to C++ template compatibility issues, but significant progress has been made:

**✅ Recently Fixed:**
- Type definitions for Linux/macOS platforms (sint32, uint32, sint64, uint64)
- Variadic macro syntax issues (changed to inline functions for PC)
- Pointer casting for 64-bit compatibility
- Added stub implementations for sound, music, and movie systems

**❌ Still Blocking:**
- C++ template syntax compatibility with modern GCC
- Missing graphics/rendering implementations
- Missing game assets

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

### What Still Needs Work

1. **C++ Template Compatibility** - The memory management templates use old MSVC syntax that doesn't compile on modern GCC
2. **Graphics Implementation** - DirectX 8 or modern OpenGL/Vulkan rendering
3. **Complete Audio Implementation** - OpenAL, FMOD, or similar
4. **Asset Pipeline** - Convert console assets to PC formats

## Build Infrastructure Added

### 1. CMake Build System

A `CMakeLists.txt` has been created that:
- Configures for PC platforms (Linux, macOS, Windows)
- Collects source files automatically from Win32/Wn32 directories
- Sets up include directories
- Provides debug/release build configurations
- **NEW**: Modular platform-specific compilation flags (see `cmake/platform_flags.cmake`)
- **NEW**: Optional feature flags for profiling, memory debugging, etc.
- **NEW**: Documentation build targets

#### Build Configuration Options

The build system now supports several configuration options:

**Build Types:**
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..      # Debug build with assertions
cmake -DCMAKE_BUILD_TYPE=Release ..    # Optimized release build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. # Release with debug info
cmake -DCMAKE_BUILD_TYPE=MinSizeRel .. # Size-optimized build
```

**Optional Features:**
```bash
# Enable Vulkan renderer (experimental)
cmake -DUSE_VULKAN_RENDERER=ON ..

# Enable performance profiling
cmake -DENABLE_PROFILING=ON ..

# Enable memory allocation debugging
cmake -DENABLE_MEMORY_DEBUG=ON ..

# Enable script system debugging
cmake -DENABLE_SCRIPT_DEBUG=ON ..
```

**Combine Options:**
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_PROFILING=ON ..
```

#### Platform Detection

The build system automatically detects:
- **Operating System**: Windows, Linux, macOS
- **Architecture**: x86 (32-bit) or x64 (64-bit)
- **Compiler**: MSVC, GCC, Clang

Platform-specific macros are automatically defined:
- `__PLAT_WN32__` - Windows platform
- `__PLAT_LINUX__` - Linux platform
- `__PLAT_MACOS__` - macOS platform
- `__ARCH_X86__` - 32-bit architecture
- `__ARCH_X64__` - 64-bit architecture

Debug builds automatically enable:
- `__NOPT_DEBUG__` - Debug mode
- `__NOPT_ASSERT__` - Enable assertions
- `__NOPT_MESSAGES__` - Enable debug messages

Release builds enable:
- `__NOPT_FINAL__` - Final release mode

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

### 4. C++ Template Compatibility Issues (BLOCKING)

The code uses older C++ template syntax that is incompatible with modern GCC:
```cpp
// Old syntax that doesn't compile on modern GCC
PtrToConst< _T >::PtrToConst< _T >( const _T* ptr )
```

**Status**: These are complex C++ template issues that require significant refactoring of the memory management and smart pointer classes. This is the current blocking issue for compilation.

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

### Prerequisites

- CMake 3.10 or later
- C++11 compatible compiler (GCC 7+, Clang 5+, MSVC 2015+)
- (Optional) Doxygen for documentation generation

### Step 1: Configure

```bash
cd /path/to/thug
mkdir build && cd build
cmake ..
```

This will succeed - CMake can configure the build and display platform information:
```
-- Platform: Linux (x64)
-- Build type: Debug
-- Compiler: GCC 13.3.0
```

You can specify build options during configuration:
```bash
# Release build with profiling
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_PROFILING=ON ..

# Debug build with memory debugging
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_MEMORY_DEBUG=ON ..
```

### Step 2: Build

```bash
cmake --build .
```

This will fail with errors like:
- "pasting '(' and '"string"' does not give a valid preprocessing token"
- "'sint32' does not name a type"
- Missing platform headers

### Step 3: Build Documentation (Always Available)

Even if the main build fails, you can build documentation:

```bash
# View available documentation files
cmake --build . --target markdown_docs

# Generate HTML documentation (if Doxygen is installed)
cmake --build . --target docs
```

The markdown documentation target shows all available documentation files and doesn't require the code to compile.

### Step 4: Understanding the Errors

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

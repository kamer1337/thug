# Building THUG - Technical Details (PC Port)

This document provides technical details about building Tony Hawk's Underground source code for PC.

## Current Status

This repository has been cleaned to focus on PC-only development. All console-specific code (PlayStation 2, GameCube, Xbox) has been removed. The code still **cannot be compiled as-is** on modern systems. This document explains why and what would be needed.

## Build Infrastructure Added

### 1. CMake Build System

A `CMakeLists.txt` has been created that:
- Configures for PC platforms (Linux, macOS, Windows)
- Collects source files automatically from Win32/Wn32 directories
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

### 3. Incomplete Win32 Implementation

The Win32/Wn32 directories contain many stub functions that were never fully implemented:
- Graphics rendering functions (return NULL or empty implementations)
- Sound system functions (stub implementations)
- Movie playback (inline stubs that do nothing)
- Memory management (basic implementations)

Many critical functions exist but don't do anything:
```cpp
inline bool PlayMovie( const char* pMovieName ) { return false; }
inline bool IsPlayingMovie( void ) { return false; }
inline void StopMovie( void ) {}
```

### 4. Missing Dependencies

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

# Tony Hawk's Underground - Source Code (PC Port)

This repository contains the source code for Tony Hawk's Underground (THUG), ported to PC only.

## Overview

This is a reverse-engineered/leaked source code of Tony Hawk's Underground that has been adapted for PC-only development. The original game was developed for PlayStation 2, GameCube, and Xbox, but all console-specific code has been removed from this repository to focus on PC development.

## Project Structure

```
Code/
├── Core/       # Core utilities, data structures, debug systems
├── Gel/        # Game Engine Layer - modules, scripting, assets
├── Gfx/        # Graphics engine and rendering
├── Sk/         # Skate-specific game logic, objects, modules
└── Sys/        # System layer - file I/O, memory, platform abstractions
```

## Building

This repository includes a CMake build system infrastructure. However, **the code cannot be built as-is** on modern systems due to several factors:

### Why It Can't Be Built Directly

1. **Missing Assets**: Game assets (models, textures, sounds, scripts) are not included
2. **Compiler Extensions**: Uses MSVC/CodeWarrior specific extensions and syntax
3. **Binary Dependencies**: Requires proprietary middleware and libraries
4. **Incomplete Windows Port**: The original Win32 port was incomplete with many stub functions

### Prerequisites (If You Had Everything)

- CMake 3.10 or higher
- Modern C++ compiler (MSVC, GCC, or Clang)
- Game assets and data files
- Proprietary middleware licenses
- Complete Windows/PC implementations for stub functions

### Build System

A CMakeLists.txt is provided as a starting point for modern compilation efforts:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

**Note**: This will fail with numerous compilation errors due to the reasons listed above. The build system is provided for documentation and as a framework for potential porting efforts.

### What Was Done to Make It "Executable"

This repository has been enhanced with:

1. **CMake Build System**: A modern `CMakeLists.txt` for cross-platform configuration
2. **Case-Insensitive Symlinks**: Symbolic links created for Linux compatibility (Windows code used case-insensitive paths)
3. **Path Separator Fixes**: Converted backslash (`\`) to forward slash (`/`) in include paths
4. **Documentation**: Comprehensive README explaining the codebase structure
5. **.gitignore**: Proper exclusion of build artifacts

### Known Issues

- Variadic macro syntax incompatible with ISO C++
- Missing fundamental type definitions (platform-specific)
- Platform-specific assembly and hardware code
- Console SDK dependencies
- Missing game assets and data files

## Important Notes

### PC-Only Port

This repository has been cleaned to focus on PC development only. All console-specific code (PS2, GameCube, Xbox) has been removed. The codebase now contains:
- `*/Win32/` or `*/Wn32/` - Windows PC implementations

### Missing Dependencies

The code still requires several components not included in this repository:
- Game assets (models, textures, sounds, scripts)
- Various middleware (sound, video, etc.)
- Complete implementations of Win32 stub functions

To fully compile and run this code, you would need:
1. Game assets (models, textures, scripts)
2. Audio libraries
3. Video codec libraries
4. Complete Win32 platform layer implementations

### Legal Disclaimer

This source code is provided for educational and preservation purposes. Tony Hawk's Underground and its source code are property of Activision and Neversoft Entertainment. Please respect intellectual property rights.

## Development

### Code Style

The codebase follows Neversoft's coding conventions:
- Classes prefixed with 'C' (e.g., `CModelBuilder`)
- Member variables prefixed with 'm_' or 'mp_' for pointers
- Namespaces for major subsystems (Gfx, Gel, Sk, etc.)

### Debug Builds

Debug builds enable assertions and debug messages. Key preprocessor flags:
- `__NOPT_DEBUG__` - Enable debug mode
- `__NOPT_ASSERT__` - Enable assertions
- `__NOPT_MESSAGES__` - Enable debug messages

## Contributing

This is an archival project. If you're working on reverse engineering or preservation:
1. Document your findings
2. Keep platform-specific code separate
3. Maintain compatibility with the original structure

## License

The original code is proprietary. This repository is for preservation and educational purposes only.

## Credits

- Original Development: Neversoft Entertainment
- Publisher: Activision
- This is part of the Tony Hawk's game series

## Documentation

For more information about the project:

- **[ROADMAP.md](ROADMAP.md)** - Development roadmap and future plans
- **[BUILDING.md](BUILDING.md)** - Detailed build instructions and technical details
- **[CHANGES.md](CHANGES.md)** - History of modifications to the codebase
- **[RENDERING_OPTIMIZATIONS.md](RENDERING_OPTIMIZATIONS.md)** - PS2 rendering performance improvements

## Resources

- [Tony Hawk's Underground on Wikipedia](https://en.wikipedia.org/wiki/Tony_Hawk%27s_Underground)
- Original release: 2003

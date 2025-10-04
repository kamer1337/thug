# Tony Hawk's Underground - Source Code

This repository contains the source code for Tony Hawk's Underground (THUG), originally developed for PlayStation 2, GameCube, and Xbox platforms.

## Overview

This is a reverse-engineered/leaked source code of Tony Hawk's Underground. The code is structured for multiple platforms including:
- PlayStation 2 (NGPS - Next Generation PlayStation)
- GameCube (NGC)
- Xbox

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

1. **Platform-Specific Dependencies**: The code was written for PlayStation 2, GameCube, and Xbox using their proprietary SDKs and hardware-specific features
2. **Console-Specific Code**: Extensive use of platform-specific libraries (VU1 code, GS registers, etc.)
3. **Missing Assets**: Game assets (models, textures, sounds, scripts) are not included
4. **Compiler Extensions**: Uses MSVC/CodeWarrior specific extensions and syntax
5. **Binary Dependencies**: Requires proprietary middleware and libraries

### Prerequisites (If You Had Everything)

- Original platform SDKs (PS2, GameCube, or Xbox)
- CMake 3.10 or higher
- Platform-specific compilers
- Game assets and data files
- Proprietary middleware licenses

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

### Platform-Specific Code

The original source code contains extensive platform-specific implementations for PS2, GameCube, and Xbox. The CMake build system currently excludes these platform-specific directories as they require the original development kits and libraries.

Platform-specific code is located in:
- `*/ngps/` or `*/NGPS/` - PlayStation 2
- `*/ngc/` or `*/NGC/` - GameCube  
- `*/xbox/` or `*/XBox/` - Xbox
- `*/Win32/` - Windows (original PC port)

### Missing Dependencies

The original code relies on proprietary platform SDKs and libraries that are not included:
- PlayStation 2 SDK (libsn, libgraph, libdma, etc.)
- GameCube/Wii SDK
- Xbox SDK
- Various middleware (sound, video, etc.)

To fully compile and run this code, you would need:
1. Original platform development kits
2. Game assets (models, textures, scripts)
3. Audio libraries
4. Video codec libraries

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

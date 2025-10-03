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

This project now includes a modern CMake build system for cross-platform compilation on modern systems.

### Prerequisites

- CMake 3.10 or higher
- A C++11 compatible compiler (GCC, Clang, MSVC)
- Make or Ninja build system

### Build Instructions

#### Linux/macOS

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# The executable will be in the build directory
./thug
```

#### Windows

```bash
# Create build directory
mkdir build
cd build

# Configure (for Visual Studio)
cmake .. -G "Visual Studio 16 2019"

# Build
cmake --build . --config Debug

# Or open the generated .sln file in Visual Studio
```

#### Build Options

- Debug build (default): `cmake .. -DCMAKE_BUILD_TYPE=Debug`
- Release build: `cmake .. -DCMAKE_BUILD_TYPE=Release`

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

## Resources

- [Tony Hawk's Underground on Wikipedia](https://en.wikipedia.org/wiki/Tony_Hawk%27s_Underground)
- Original release: 2003

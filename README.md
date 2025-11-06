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
│   └── Vulcan/ # Vulkan renderer (fully documented, see docs/VULKAN_RENDERER.md)
├── Sk/         # Skate-specific game logic, objects, modules
└── Sys/        # System layer - file I/O, memory, platform abstractions
```

## Features

### Vulkan Renderer 🎨

This repository includes a complete Vulkan renderer implementation with:
- ✅ Full API documentation and integration guide
- ✅ 10 working code examples
- ✅ CMake build integration
- ✅ Complete software architecture
- 🔨 GPU operations stubbed (ready for Vulkan API implementation)

See **[docs/VULKAN_RENDERER.md](docs/VULKAN_RENDERER.md)** for complete details.

Quick start: `Code/Gfx/Vulcan/QUICK_REFERENCE.md`

### PC Controls ⌨️

This PC port now includes keyboard controls! See [PC Keyboard Controls Documentation](docs/PC_KEYBOARD_CONTROLS.md) for the complete control scheme.

**Quick Reference:**
- Movement: WASD or Arrow Keys
- Jump: Space
- Grind/Grab: Shift
- Flip Tricks: Ctrl
- Special: Alt
- Camera: I/J/K/L
- Pause: Enter

## Building

This repository includes a CMake build system infrastructure. **Significant progress has been made on PC porting**, but compilation is currently blocked by C++ template compatibility issues.

### Recent PC Port Progress ✨

**What's Been Fixed:**
- ✅ Type definitions for Linux/macOS (sint32, uint32, sint64, uint64)
- ✅ Variadic macro syntax compatibility
- ✅ 64-bit pointer casting issues
- ✅ Audio/video stub implementations (sound, music, movies)
- ✅ Platform detection for Linux/Mac

**What's Blocking Compilation:**
- ❌ C++ template compatibility with modern GCC (requires refactoring memory management)

See **[docs/PC_PORT_PROGRESS.md](docs/PC_PORT_PROGRESS.md)** for detailed progress report.

### Why It Can't Be Fully Built Yet

1. **C++ Template Issues**: Old template syntax incompatible with modern GCC (blocking compilation)
2. **Missing Assets**: Game assets (models, textures, sounds, scripts) are not included - **See [Data Structure](#data-structure-and-assets)**
3. **Graphics Backend**: Needs DirectX/OpenGL/Vulkan implementation
4. **Audio Backend**: Needs OpenAL/FMOD/SDL_mixer implementation

**See [EXECUTABLE_COMPLETION.md](EXECUTABLE_COMPLETION.md) for a comprehensive list of TODO tasks to make the executable functional.**

## Data Structure and Assets

This repository includes a proper PC-compatible asset directory structure but does **not** include actual game assets due to copyright.

**Asset Directories:**

1. **`pre/`** - PRE archive files (original compressed archives)
   - Place `.pre` files from original game here
   - Engine loads assets directly from these archives
   - See `pre/README.md` for details

2. **`Data/`** - Extracted asset files (for development)
   - Individual asset files organized by type
   - Easier to modify and test
   - See `Data/README.md` for structure

**Asset Structure**: `Data/`
```
Data/
├── README.md              # Asset directory documentation
├── ASSET_MANIFEST.md      # Complete list of required assets
├── Models/                # 3D models (.mdl files)
│   ├── GameObjects/       # SKATE/COMBO letters, collectibles
│   ├── Characters/        # Skater and NPC models
│   ├── Vehicles/          # Cars and vehicles
│   └── Props/             # Level props and objects
├── Textures/              # Texture files (.tex, .img)
├── Scenes/                # Level geometry (.scn)
├── Animations/            # Animation data (.ska)
├── Audio/                 # Music and sound effects
├── Scripts/               # QB script files (.qb)
└── Config/                # Configuration files
```

**PRE Archive Structure**: `pre/`
```
pre/
├── README.md              # PRE archive documentation
├── qb.pre                 # QB script archives
├── skaterparts.pre        # Skater customization
├── levels.pre             # Level geometry
├── models.pre             # Character models
└── ... (other .pre files from original game)
```

**To obtain assets:**
1. You must own a legal copy of Tony Hawk's Underground
2. **Option A:** Copy `.pre` files to `pre/` directory (quick setup)
3. **Option B:** Extract assets to `Data/` using tools in `tools/` directory
4. See `docs/ASSET_EXTRACTION.md` for detailed instructions
5. See `docs/ASSET_FORMATS.md` for technical format details

**Asset Documentation:**
- 📦 **[pre/README.md](pre/README.md)** - PRE archive files documentation
- 📁 **[Data/README.md](Data/README.md)** - Complete directory structure and conventions
- 📄 **[Data/ASSET_MANIFEST.md](Data/ASSET_MANIFEST.md)** - Full list of required assets
- 🔧 **[docs/ASSET_EXTRACTION.md](docs/ASSET_EXTRACTION.md)** - How to extract from original game
- 📋 **[docs/ASSET_FORMATS.md](docs/ASSET_FORMATS.md)** - Detailed file format specifications

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

# Standard build
cmake ..

# Or enable Vulkan renderer (experimental)
cmake -DUSE_VULKAN_RENDERER=ON ..

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
- Game assets (models, textures, sounds, scripts) - **See [Data Structure and Assets](#data-structure-and-assets)**
- Various middleware (sound, video, etc.)
- Complete implementations of Win32 stub functions

To fully compile and run this code, you would need:
1. **Game assets** - Extract from original THUG PC installation using tools in `tools/` directory
   - See `docs/ASSET_EXTRACTION.md` for extraction guide
   - See `Data/ASSET_MANIFEST.md` for complete asset list
   - See `docs/ASSET_FORMATS.md` for format specifications
2. **Audio libraries** - OpenAL, FMOD, or SDL2_mixer
3. **Video codec libraries** - For cutscenes
4. **Complete Win32 platform layer implementations**

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

### Main Documentation
- **[ROADMAP.md](ROADMAP.md)** - Development roadmap and future plans
- **[TASKS.md](TASKS.md)** - ⭐ Discrete tasks ready for pull requests!
- **[EXECUTABLE_COMPLETION.md](EXECUTABLE_COMPLETION.md)** - ⭐ TODO tasks to make the executable compile and run
- **[BUILDING.md](BUILDING.md)** - Detailed build instructions and technical details
- **[CHANGES.md](CHANGES.md)** - History of modifications to the codebase
- **[LEGAL.md](LEGAL.md)** - Legal information, disclaimers, and copyright notices
- **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** - ⭐ System architecture and design overview
- **[docs/GAME_LOOP.md](docs/GAME_LOOP.md)** - Frame processing and update cycle details
- **[docs/VULKAN_RENDERER.md](docs/VULKAN_RENDERER.md)** - Vulkan renderer documentation and integration guide
- **[docs/THUG2_QUICK_REFERENCE.md](docs/THUG2_QUICK_REFERENCE.md)** - ⭐ Quick THUG vs THUG2 comparison guide
- **[docs/THUG2_RESEARCH.md](docs/THUG2_RESEARCH.md)** - THUG2 game similarities and comparison analysis
- **[docs/THUG2_IMPLEMENTATION.md](docs/THUG2_IMPLEMENTATION.md)** - Technical guide for implementing THUG2 features
- **[docs/subsystems/RECORDS.md](docs/subsystems/RECORDS.md)** - Records system documentation

### Asset Documentation
- **[Data/README.md](Data/README.md)** - ⭐ Complete asset directory structure and conventions
- **[Data/ASSET_MANIFEST.md](Data/ASSET_MANIFEST.md)** - ⭐ Full list of required game assets
- **[docs/ASSET_EXTRACTION.md](docs/ASSET_EXTRACTION.md)** - ⭐ Guide to extracting assets from original game
- **[docs/ASSET_FORMATS.md](docs/ASSET_FORMATS.md)** - ⭐ Detailed file format specifications (.mdl, .tex, .scn, etc.)

### Vulkan Renderer Documentation

The Vulkan renderer is fully documented with multiple resources:
- `Code/Gfx/Vulcan/QUICK_REFERENCE.md` - Quick API reference
- `Code/Gfx/Vulcan/INTEGRATION.md` - Comprehensive integration guide
- `Code/Gfx/Vulcan/example.cpp` - 10 working examples
- `Code/Gfx/Vulcan/IMPLEMENTATION.md` - Implementation status details
- `docs/VULKAN_RENDERER.md` - Complete summary

### Tools

Standalone tools for working with THUG assets:
- **[tools/README.md](tools/README.md)** - Overview of all available tools
- **[tools/model_viewer/](tools/model_viewer/)** - ⭐ 3D Model, Skeleton & Animation Viewer/Editor

## Resources

- [Tony Hawk's Underground on Wikipedia](https://en.wikipedia.org/wiki/Tony_Hawk%27s_Underground)
- Original release: 2003

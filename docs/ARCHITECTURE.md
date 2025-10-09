# THUG Architecture Documentation

## Overview

Tony Hawk's Underground (THUG) is built on the Neversoft Game Engine, a sophisticated multi-layered architecture designed for console and PC gaming. This document provides a comprehensive overview of the engine's architecture, subsystems, and data flow.

## Table of Contents

- [High-Level Architecture](#high-level-architecture)
- [Layer Structure](#layer-structure)
- [Core Subsystems](#core-subsystems)
- [Data Flow](#data-flow)
- [Initialization Pipeline](#initialization-pipeline)
- [Game Loop](#game-loop)
- [Memory Architecture](#memory-architecture)
- [Threading Model](#threading-model)
- [Platform Abstraction](#platform-abstraction)

---

## High-Level Architecture

The THUG engine follows a layered architecture pattern with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────┐
│                    Sk/ (Skate Layer)                    │
│         Game-Specific Logic & Gameplay Systems          │
├─────────────────────────────────────────────────────────┤
│                    Gel/ (Game Engine Layer)             │
│    Modules, Scripting, Assets, Collision, Objects      │
├─────────────────────────────────────────────────────────┤
│                    Gfx/ (Graphics Layer)                │
│         Rendering, 2D/3D Graphics, Vulkan Support       │
├─────────────────────────────────────────────────────────┤
│                 Core/ (Core Utilities)                  │
│    Data Structures, Math, Debug, Threads, Tasks        │
├─────────────────────────────────────────────────────────┤
│                    Sys/ (System Layer)                  │
│    Platform Abstraction, File I/O, Memory, Config      │
└─────────────────────────────────────────────────────────┘
```

### Design Philosophy

1. **Separation of Concerns**: Each layer has distinct responsibilities
2. **Bottom-Up Dependencies**: Upper layers depend on lower layers, never vice versa
3. **Platform Independence**: System layer abstracts platform-specific code
4. **Component-Based**: Game objects use component composition
5. **Data-Driven**: Heavy use of scripting and data files

---

## Layer Structure

### 1. System Layer (Sys/)

**Purpose**: Platform abstraction and low-level system services

**Key Responsibilities**:
- File I/O operations (`Sys/File/`)
- Memory management (`Sys/Mem/`)
- Platform-specific implementations (`Sys/Win32/`)
- Configuration management (`Sys/Config/`)
- Replay system (`Sys/Replay/`)
- Memory card operations (`Sys/MemCard/`)

**Platform Support**:
- Win32: Windows PC implementation
- Platform-specific code isolated in subdirectories

### 2. Core Layer (Core/)

**Purpose**: Engine-agnostic utilities and fundamental data structures

**Key Subsystems**:
- **Math** (`Core/Math/`): Vector, matrix, quaternion operations
- **String** (`Core/String/`): String manipulation utilities
- **Debug** (`Core/Debug/`): Assertions, logging, profiling
- **Task** (`Core/Task/`): Task scheduling system
- **Thread** (`Core/Thread/`): Threading primitives
- **List** (`Core/List/`): List and collection data structures
- **Support** (`Core/Support/`): General utility functions

**Design Pattern**: No game-specific logic; purely utility code

### 3. Graphics Layer (Gfx/)

**Purpose**: Rendering pipeline and graphics management

**Key Components**:
- **2D Graphics** (`Gfx/2D/`): UI, HUD, menu rendering
- **Image** (`Gfx/Image/`): Texture loading and management
- **Win32** (`Gfx/Win32/`): DirectX 8 implementation
- **Vulkan** (`Gfx/Vulcan/`): Modern Vulkan renderer (see [VULKAN_RENDERER.md](VULKAN_RENDERER.md))
- **NX API**: Neversoft's graphics abstraction layer

**Architecture**:
- Hardware abstraction through NX API
- Platform-specific backends (DirectX, Vulkan)
- Scene management and rendering pipeline

### 4. Game Engine Layer (Gel/)

**Purpose**: Game engine services and middleware

**Major Subsystems**:
- **MainLoop** (`Gel/MainLoop/`): Core game loop and frame processing
- **Scripting** (`Gel/Scripting/`): QB scripting language integration
- **Object** (`Gel/Object/`): Game object management
- **Components** (`Gel/Components/`): Component-based object system
- **Collision** (`Gel/Collision/`): Collision detection system
- **AssMan** (`Gel/AssMan/`): Asset management
- **Input** (`Gel/Input/`): Input handling
- **Module** (`Gel/Module/`): Modular subsystem management
- **Net** (`Gel/Net/`): Networking infrastructure
- **Music** (`Gel/Music/`): Music playback system
- **SoundFX** (`Gel/SoundFX/`): Sound effects
- **Movies** (`Gel/Movies/`): Video playback
- **Environment** (`Gel/Environment/`): Environment effects
- **Prefs** (`Gel/Prefs/`): User preferences

**Design Pattern**: Component-based object system with scripting integration

### 5. Skate Layer (Sk/)

**Purpose**: Game-specific logic for Tony Hawk's Underground

**Major Systems**:
- **Engine** (`Sk/Engine/`): Core skateboarding mechanics
- **Objects** (`Sk/Objects/`): Game-specific objects
  - Skater (`skater.h`, `SkaterTricks.h`)
  - Camera (`skatercam.h`)
  - Career mode (`skatercareer.h`)
  - Trick system (`TrickObject.h`, `manual.h`)
  - Records (`records.h`)
- **Components** (`Sk/Components/`): Skate-specific components
- **Modules** (`Sk/Modules/`): Game modules
- **Scripting** (`Sk/Scripting/`): Game-specific script commands
- **GameNet** (`Sk/GameNet/`): Multiplayer networking
- **ParkEditor** (`Sk/ParkEditor/`, `Sk/ParkEditor2/`): Level editor

---

## Core Subsystems

### Component System

The engine uses a **component-based architecture** for game objects:

```
CCompositeObject
    └── Components[]
        ├── CAnimComponent       (Animation)
        ├── CModelComponent      (3D Model)
        ├── CSkeletonComponent   (Skeletal Animation)
        ├── CSoundComponent      (Audio)
        ├── CInputComponent      (Player Input)
        └── [Custom Components]
```

**Benefits**:
- Flexible object composition
- Code reuse across different object types
- Easy addition of new behaviors

### Scripting System

The **QB (QuickByte) scripting language** is integral to THUG:

- **Script Execution**: Compiled bytecode executed by interpreter
- **Integration**: Scripts can call C++ functions and vice versa
- **Usage**: Game logic, cutscenes, UI, goals, missions
- **Symbol Table**: Manages script functions and variables

**Key Files**: `Gel/Scripting/symboltable.h`, `Sk/Scripting/`

### Task System

**Task-based execution model** for managing concurrent operations:

- Tasks organized in priority stacks
- Frame-based processing
- Used for animations, updates, rendering
- System tasks vs. game tasks

**Key Files**: `Core/Task/`, referenced in `Gel/MainLoop/Mainloop.cpp`

### Collision System

Multi-phase collision detection:

1. **Broad Phase**: Spatial partitioning
2. **Narrow Phase**: Precise collision tests
3. **Response**: Physics and game logic response

**Key Files**: `Gel/Collision/`

---

## Data Flow

### Frame Data Flow

```
Input → Physics → Game Logic → AI → Animation → Rendering → Audio
   ↓                  ↓            ↓        ↓         ↓         ↓
   └──────────────────┴────────────┴────────┴─────────┴─────────┘
                         Event System
```

### Asset Loading Flow

```
Game Scripts → Asset Manager → File System → Memory Allocator
                      ↓
              ┌───────┴────────┐
              ↓                ↓
         Textures          Models
              ↓                ↓
         Graphics         Animation
          Pipeline          System
```

### Component Update Flow

```
Frame Start
    ↓
System Tasks (Input, Network)
    ↓
Game Object Updates
    ↓
Component Updates (per object)
    ├── Input Component
    ├── Physics Component
    ├── Animation Component
    └── [Other Components]
    ↓
Collision Detection
    ↓
Render Scene
    ↓
Frame End
```

---

## Initialization Pipeline

### Engine Startup Sequence

1. **System Initialization** (`Sys/`)
   - Platform initialization
   - Memory system setup
   - File system initialization

2. **Core Initialization** (`Core/`)
   - Debug system
   - Task manager
   - Thread system

3. **Graphics Initialization** (`Gfx/`)
   - Create rendering device
   - Load shaders/materials
   - Initialize scene manager

4. **Engine Initialization** (`Gel/`)
   - Asset manager
   - Object manager
   - Component system
   - Scripting engine
   - Input system
   - Audio system

5. **Game Initialization** (`Sk/`)
   - Load game scripts
   - Initialize game modules
   - Create initial game objects
   - Load first level

### Shutdown Sequence

Reverse order of initialization, ensuring clean resource cleanup.

---

## Game Loop

See [GAME_LOOP.md](GAME_LOOP.md) for detailed game loop documentation.

### Basic Structure

```cpp
while (!done) {
    // 1. Service system tasks
    service_system();
    
    // 2. Process input
    process_input();
    
    // 3. Update game state
    update_game_logic();
    
    // 4. Update physics
    update_physics();
    
    // 5. Update animation
    update_animation();
    
    // 6. Render frame
    render_scene();
    
    // 7. Update audio
    update_audio();
    
    // 8. Sync frame timing
    sync_frame();
}
```

**Key File**: `Gel/MainLoop/Mainloop.cpp`

---

## Memory Architecture

### Memory Management Strategy

1. **Custom Allocators**: Pool allocators for specific object types
2. **Heap Management**: Platform-specific heap implementations
3. **Memory Tracking**: Debug builds track allocations
4. **Memory Cards**: Save game management (console legacy)

**Key Files**: `Sys/Mem/`, `Sk/heap_sizes.h`

### Memory Pools

Different pools for different object types:
- Script heap
- Geometry heap
- Animation heap
- Audio heap
- Network heap

### Memory Constraints

Originally designed for console memory constraints (32-64MB):
- Careful memory management
- Streaming systems for large data
- Asset compression

---

## Threading Model

### Task-Based Concurrency

THUG uses a **task-based model** rather than traditional threading:

- Tasks scheduled on frame boundaries
- Priority-based execution
- Platform-specific thread implementations

**Key Files**: `Core/Thread/`, `Core/Task/`

### Profiling Support

Built-in profiler for performance analysis:
- CPU profiling contexts
- Task timing
- Memory profiling

**Usage**: `__USE_PROFILER__` preprocessor flag

---

## Platform Abstraction

### Platform-Specific Code Organization

```
Code/
├── Sys/Win32/     # Windows system layer
├── Gfx/Win32/     # DirectX 8 graphics
└── [Other]/       # Platform-neutral code
```

### Platform Macros

- `__PLAT_WN32__` or `__PLAT_WIN32__`: Windows PC
- Platform checks isolate platform-specific code

### Stub Functions

Many Win32 functions are stubbed (not fully implemented). See [platforms/STUB_FUNCTIONS.md](platforms/STUB_FUNCTIONS.md) for complete list.

---

## Key Design Patterns

### 1. Singleton Pattern

Used for manager classes:
```cpp
DefineSingletonClass( Manager, "Main Loop Manager" );
```

**Examples**: Main Loop Manager, Asset Manager, Object Manager

### 2. Component Pattern

Game objects composed of components:
- Flexible composition
- Easy to extend
- Good code reuse

### 3. Observer Pattern

Event system for loose coupling:
- Script events
- Game events
- Input events

### 4. Factory Pattern

Object creation through factories:
- Component factories
- Object factories
- Asset loaders

### 5. State Pattern

Used extensively for:
- Game states (menu, gameplay, pause)
- Character states (skating, grinding, manual)
- UI states

---

## Module Dependencies

### Dependency Graph

```
Sk/ ──→ Gel/ ──→ Gfx/ ──→ Core/ ──→ Sys/
  │       │        │        │
  │       │        └────────┘
  │       └─────────────────┘
  └─────────────────────────┘
```

**Rule**: Layers only depend on layers below them

### Cross-Cutting Concerns

Some systems span multiple layers:
- **Debug**: Used by all layers
- **Memory**: Managed at Sys level, used everywhere
- **Profiling**: Instrumentation across all layers

---

## Build System

### CMake Structure

See [BUILDING.md](../BUILDING.md) for complete build instructions.

**Directory Structure**:
```
CMakeLists.txt          # Root build configuration
Code/
  ├── CMakeLists.txt    # (Could be added for modular builds)
  └── [subsystems]/
```

### Build Targets

- Main executable
- Subsystem libraries
- Optional Vulkan renderer (`USE_VULKAN_RENDERER=ON`)

### Conditional Compilation

Key preprocessor flags:
- `__NOPT_DEBUG__`: Debug mode
- `__NOPT_ASSERT__`: Assertions enabled
- `__PLAT_WN32__`: Windows platform
- `__USE_PROFILER__`: Profiling enabled

---

## Asset Pipeline

### Asset Types

1. **3D Models**: Character models, level geometry
2. **Textures**: Surfaces, UI elements
3. **Animations**: Character animations, cutscenes
4. **Audio**: Music, sound effects, voice
5. **Scripts**: Game logic, missions, UI
6. **Level Data**: Collision, spawn points, triggers

### Asset Loading

- Asynchronous loading for streaming
- Managed by Asset Manager (`Gel/AssMan/`)
- Platform-specific file formats

---

## Performance Considerations

### Optimization Strategies

1. **Memory Management**: Custom allocators, pool allocators
2. **Culling**: Frustum culling, occlusion culling
3. **LOD**: Level of detail for models
4. **Streaming**: Load assets on-demand
5. **Batching**: Reduce draw calls
6. **Task Scheduling**: Efficient CPU utilization

### Profiling

Use built-in profiler to identify bottlenecks:
- CPU profiling per task
- Memory usage tracking
- Frame timing analysis

---

## Extension Points

### Adding New Features

1. **New Component**: Extend component system
2. **New Script Command**: Register with script engine
3. **New Object Type**: Derive from CCompositeObject
4. **New Module**: Add to module system
5. **New Asset Type**: Extend asset manager

### THUG2 Extensions

For implementing THUG2 features, see:
- [THUG2_IMPLEMENTATION.md](THUG2_IMPLEMENTATION.md)
- [THUG2_RESEARCH.md](THUG2_RESEARCH.md)

---

## Code Organization Best Practices

### Naming Conventions

- **Classes**: `CClassName` (prefix with 'C')
- **Members**: `m_memberName` (prefix with 'm_')
- **Pointers**: `mp_pointerName` (prefix with 'mp_')
- **Globals**: `g_globalName` (prefix with 'g_')

See [CODE_STYLE.md](CODE_STYLE.md) for complete style guide.

### File Organization

- Header files: `.h`
- Implementation files: `.cpp`
- Platform-specific: `[subsystem]/Win32/`
- One class per file (generally)

---

## Related Documentation

- **[GAME_LOOP.md](GAME_LOOP.md)** - Detailed game loop documentation
- **[SCRIPTING.md](SCRIPTING.md)** - QB scripting system *(To be created)*
- **[RENDERING.md](RENDERING.md)** - Graphics pipeline *(To be created)*
- **[platforms/WIN32.md](platforms/WIN32.md)** - Windows platform specifics
- **[VULKAN_RENDERER.md](VULKAN_RENDERER.md)** - Vulkan renderer documentation
- **[subsystems/](subsystems/)** - Individual subsystem documentation

---

## Future Improvements

### Planned Enhancements

1. **HAL Layer**: Hardware Abstraction Layer for better platform support
2. **Modern C++**: Upgrade to C++11/14/17 standards
3. **Unit Tests**: Test coverage for core systems
4. **Documentation**: Complete subsystem documentation
5. **Tooling**: Asset extraction and conversion tools

See [ROADMAP.md](../ROADMAP.md) for complete development plan.

---

## Conclusion

THUG's architecture demonstrates excellent software engineering for its era:
- Clean layer separation
- Component-based design
- Platform abstraction
- Scripting integration
- Performance-focused

Understanding this architecture is key to:
- Implementing new features
- Porting to new platforms
- Debugging issues
- Learning game engine design

---

## References

- **Source Code**: `Code/` directory
- **Original Development**: Neversoft Entertainment (1999-2004)
- **Engine**: Neversoft Game Engine
- **Game**: Tony Hawk's Underground (2003)

---

*Last Updated: 2024-10*
*Version: 1.0*

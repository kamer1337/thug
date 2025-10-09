# Hardware Abstraction Layer (HAL)

The Hardware Abstraction Layer provides platform-independent interfaces for hardware-specific functionality.

## Purpose

The HAL allows THUG to run on multiple platforms without changing game code. Platform-specific implementations are hidden behind abstract interfaces.

## Current Modules

### Audio (`Audio.h`, `Audio.cpp`)

Cross-platform audio system supporting multiple backends:
- NULL backend (no audio)
- OpenAL backend (3D audio)
- SDL2_mixer backend (simple audio) - planned

**Key Features:**
- Sound effect playback (32 voices)
- Music streaming (4 channels)
- 3D positional audio
- Volume and pitch control

**Usage:**
```cpp
#include <core/HAL/Audio.h>

// Create audio device
HAL::AudioDevice* device = HAL::CreateAudioDevice(HAL::AUDIO_BACKEND_OPENAL);
device->Initialize();

// Use audio device...
device->Update();

// Cleanup
device->Shutdown();
HAL::DestroyAudioDevice(device);
```

## Audio Backends

### OpenAL (`AudioOpenAL.h`, `AudioOpenAL.cpp`)

OpenAL is a cross-platform 3D audio API. It provides:
- Hardware-accelerated audio
- 3D spatial audio
- Multiple simultaneous sound sources
- Doppler effect support

**Compile-time option:** `-DUSE_OPENAL=ON`

**Dependencies:**
- libopenal (Linux/Windows)
- OpenAL.framework (macOS)

### SDL2_mixer (Planned)

SDL2_mixer is a simple audio mixing library. It provides:
- Easy-to-use API
- Multiple format support
- Music and sound effects
- Cross-platform compatibility

**Compile-time option:** `-DUSE_SDL_MIXER=ON` (future)

**Dependencies:**
- libsdl2
- libsdl2-mixer

## Adding New Backends

To add a new audio backend:

1. Create interface implementation:
```cpp
// Code/Core/HAL/AudioMyBackend.h
class MyBackendAudioDevice : public HAL::AudioDevice {
    // Implement all virtual methods...
};
```

2. Update factory in `Audio.cpp`:
```cpp
case AUDIO_BACKEND_MYBACKEND:
#ifdef USE_MYBACKEND
    device = new MyBackendAudioDevice();
#endif
    break;
```

3. Add CMake option:
```cmake
option(USE_MYBACKEND "Use MyBackend audio" OFF)
if(USE_MYBACKEND)
    add_definitions(-DUSE_MYBACKEND)
endif()
```

## Design Principles

1. **Platform Independence**: Game code should never directly call platform APIs
2. **Runtime Selection**: Backend can be chosen at runtime (where possible)
3. **Graceful Degradation**: If a backend fails, fall back to NULL backend
4. **Consistent Interface**: All backends implement the same interface
5. **Performance**: Minimal overhead compared to direct API calls

## Future Modules

Planned HAL modules include:

- **Graphics** - Abstract rendering API (OpenGL, DirectX, Vulkan)
- **Input** - Abstract input handling (keyboard, mouse, gamepad)
- **File I/O** - Abstract file system operations
- **Network** - Abstract network operations
- **Threading** - Abstract thread and synchronization primitives

## Related Documentation

- [Audio System Documentation](../../docs/subsystems/AUDIO.md)
- [Platform Documentation](../../docs/platforms/)
- [Build Instructions](../../BUILDING.md)
- [Development Tasks](../../TASKS.md)

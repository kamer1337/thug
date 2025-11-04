# Audio System

## Overview

The THUG audio system handles music playback, sound effects, and positional audio. The system is implemented in `Code/Gel/SoundFX/` and `Code/Gel/Music/` with platform-specific implementations for different target platforms.

### Audio Backend Architecture (PC Port)

The PC port now supports multiple audio backends through a compile-time configuration system:

**Available Backends**:
- **SDL2_mixer** - Recommended, simple API, cross-platform, free
- **OpenAL** - 3D positional audio, cross-platform, free
- **FMOD** - Professional features, requires commercial license

**Configuration**: Select backend via CMake:
```bash
cmake -DAUDIO_BACKEND=SDL2    # SDL2_mixer (recommended)
cmake -DAUDIO_BACKEND=OpenAL  # OpenAL
cmake -DAUDIO_BACKEND=FMOD    # FMOD
cmake -DAUDIO_BACKEND=None    # No backend (stubs only)
```

**Implementation Location**:
- `Code/Gel/Music/Win32/p_music.h/cpp` - Platform dispatcher layer
- `Code/Gel/Music/SDL2/p_audio.h/cpp` - SDL2_mixer backend (stub)
- `Code/Gel/Music/OpenAL/p_audio.h/cpp` - OpenAL backend (stub)
- `Code/Gel/Music/FMOD/p_audio.h/cpp` - FMOD backend (stub)

The Win32 platform layer now acts as a dispatcher, routing audio calls to the selected backend based on preprocessor defines (`USE_SDL2_AUDIO`, `USE_OPENAL_AUDIO`, `USE_FMOD_AUDIO`).

## Architecture

### Core Components

#### 1. Sound Effects System (`Code/Gel/SoundFX/`)

The SoundFX subsystem manages short audio samples and positional sounds:

**Key Files**:
- `soundfx.h/cpp` - Core sound effects interface
- `win32/p_sfx.h/cpp` - Win32 platform implementation (DirectSound-based)

**Features**:
- Wave table management (non-permanent and permanent sounds)
- 3D positional audio
- Distance-based attenuation
- Volume and pitch control
- Looping sounds
- Sound streaming

#### 2. Music System (`Code/Gel/Music/`)

Handles music playback and streaming:

**Key Files**:
- Music management and streaming
- Platform-specific music decoders
- Playlist management
- Cross-fade support

#### 3. Sound Components

Object-based sound management through components:

**Sound Component** (`Code/Gel/Components/soundcomponent.h/cpp`):
- Attach sounds to game objects
- Automatic position updates
- Object lifetime management

**Skater Sound Component** (`Code/Sk/Components/SkaterSoundComponent.h/cpp`):
- Skateboard-specific sound effects
- Trick sounds
- Movement sounds (rolling, landing)

**Skater Looping Sound Component** (`Code/Sk/Components/SkaterLoopingSoundComponent.h/cpp`):
- Continuous skateboard sounds
- Speed-based sound modulation
- Smooth volume transitions

**Vehicle Sound Component** (`Code/Gel/Components/VehicleSoundComponent.h/cpp`):
- Vehicle-specific audio
- Engine sounds
- Speed-based pitch modulation

## Sound System Configuration

### Constants and Limits

From `soundfx.h`:

```cpp
#define WAVE_TABLE_MAX_ENTRIES          256  // Non-permanent sounds
#define PERM_WAVE_TABLE_MAX_ENTRIES     256  // Permanent sounds
#define MAX_POSITIONAL_SOUNDS           128  // Max 3D positioned sounds

#define DEFAULT_DROPOFF_DIST  FEET_TO_INCHES(85.0f)  // Default hearing distance
```

### Compilation Options

```cpp
#define NO_SOUND_PLEASE  0  // Set to 1 to compile out sound system
```

## Positional Audio

### 3D Sound Positioning

The system supports full 3D positional audio:

**Features**:
- Distance-based volume attenuation
- Doppler effect (speed-based pitch shift)
- Sound obstruction/occlusion
- Listener orientation
- Sound cone directionality

### Distance Calculation

```cpp
// Distance at which to start playing a sound
#define DIST_FROM_DROPOFF_AT_WHICH_TO_START_SOUND  FEET_TO_INCHES(20.0f)

// Hysteresis buffer to prevent rapid on/off switching
#define SOUND_DIST_BUFFER  FEET_TO_INCHES(20.0f)

// Total stop distance
#define DIST_FROM_DROPOFF_AT_WHICH_TO_STOP_SOUND  (SOUND_DIST_BUFFER + START_SOUND_DIST)
```

**Purpose of Buffer**:
Prevents sounds from rapidly switching on/off when the player is near the audible range boundary, which would cause audio popping.

### Volume Phasing

Some platforms support volume phasing for directional audio:

```cpp
#if defined(__PLAT_NGPS__) || defined(__PLAT_XBOX__) || defined(__PLAT_WN32__)
#define PLATFORM_SUPPORTS_VOLUME_PHASING  1
#endif
```

When enabled, negative volume values can create "out of phase" audio to simulate sounds behind the listener.

## Sound Categories

### Sound Types

1. **Non-Permanent Sounds**:
   - Temporary sound effects
   - One-shot sounds (impacts, jumps)
   - Limited to WAVE_TABLE_MAX_ENTRIES active sounds

2. **Permanent Sounds**:
   - Always loaded sounds
   - UI sounds
   - Critical gameplay sounds
   - Limited to PERM_WAVE_TABLE_MAX_ENTRIES

3. **Streaming Sounds**:
   - Music tracks
   - Long sound effects
   - Ambient loops
   - Voice-overs (if implemented)

### Skateboarding Sounds

#### Movement Sounds
- **Rolling**: Continuous loop based on speed and terrain
- **Landing**: Impact sounds based on fall height
- **Grinding**: Rail/surface-specific grinding sounds
- **Sliding**: Board sliding sounds
- **Wallride**: Wall contact sounds

#### Trick Sounds
- **Ollies**: Jump/pop sounds
- **Flip Tricks**: Board flipping sounds
- **Grab Tricks**: Grab/catch sounds
- **Rotation**: Spin whoosh sounds
- **Bails**: Crash and fall sounds

#### Terrain-Specific Sounds
Different sounds for different surface types:
- Concrete
- Wood
- Metal
- Grass
- Gravel
- etc.

## Platform Implementation

### Audio Backend Implementation (PC Port)

Located in `Code/Gel/Music/`:

**Platform Dispatcher** (`Win32/p_music.h/cpp`):
- Routes audio calls to selected backend
- Compile-time backend selection via preprocessor defines
- Falls back to stubs when no backend is selected

**SDL2 Backend** (`SDL2/p_audio.h/cpp`):
- Simple, cross-platform audio library
- Easy integration with SDL2_mixer
- Good for basic music and sound playback
- **Status**: Stub implementation, needs SDL2_mixer integration

**OpenAL Backend** (`OpenAL/p_audio.h/cpp`):
- 3D positional audio support
- Cross-platform, free and open-source
- Best for spatial audio and sound effects
- **Status**: Stub implementation, needs OpenAL-soft integration

**FMOD Backend** (`FMOD/p_audio.h/cpp`):
- Professional audio middleware
- Advanced features and DSP effects
- Requires commercial license for releases
- **Status**: Stub implementation, needs FMOD SDK integration

**Backend Selection**:
Configure via CMake at build time:
```bash
cmake -DAUDIO_BACKEND=SDL2    # Uses USE_SDL2_AUDIO define
cmake -DAUDIO_BACKEND=OpenAL  # Uses USE_OPENAL_AUDIO define
cmake -DAUDIO_BACKEND=FMOD    # Uses USE_FMOD_AUDIO define
```

### Win32/DirectSound Implementation (Legacy)

Located in `Code/Gel/SoundFX/win32/p_sfx.h/cpp`:

**Note**: The DirectSound-based implementation is for the original Windows build. The PC port now uses the modern audio backend system described above (SDL2/OpenAL/FMOD).

**Platform-Specific Features** (Original DirectSound):
- DirectSound integration
- 3D sound buffer management
- Hardware acceleration support
- Software mixing fallback

**Stub Functions** (see [STUB_FUNCTIONS.md](../platforms/STUB_FUNCTIONS.md)):
Many Win32 audio functions are currently stubs that need implementation.

### Audio Device Management

```cpp
namespace NxWn32
{
    class CAudioDevice
    {
        // Initialize DirectSound
        void Initialize();
        
        // Shutdown audio system
        void Shutdown();
        
        // Per-frame audio update
        void Update();
    };
}
```

## Sound Playback Control

### Basic Playback

```cpp
// Play a sound effect
PlaySound(soundId, volume, pitch, priorit y);

// Play a positional sound
PlayPositionalSound(soundId, position, volume, pitch, dropoff_dist);

// Stop a sound
StopSound(soundId);

// Stop all sounds
StopAllSounds();
```

### Looping Sounds

```cpp
// Start a looping sound
PlayLoopingSound(soundId, volume, pitch);

// Update looping sound parameters
UpdateLoopingSound(soundId, new_volume, new_pitch);

// Stop looping sound
StopLoopingSound(soundId);
```

### Sound Modulation

**Volume Control**:
- Master volume
- Category volumes (SFX, music, voice)
- Per-sound volume
- Distance attenuation

**Pitch Control**:
- Base pitch
- Speed-based pitch (Doppler)
- Random pitch variation
- Per-sound pitch override

## Music System

### Music Playback

**Features**:
- Streaming music playback
- Playlist management
- Track transitions
- Cross-fading between tracks
- Volume ducking for voice-overs

### Soundtrack Integration

The THUG series is known for its licensed soundtracks:

**Considerations**:
- Music file formats
- Streaming from disk
- Memory management
- Platform-specific decoders

## Integration with Game Systems

### Physics Integration

Sound events triggered by physics:
- Collision sounds (impact velocity-based)
- Material-based collision sounds
- Friction/sliding sounds
- Environment interaction sounds

### Animation Integration

Sound events triggered by animations:
- Footstep sounds
- Animation event markers trigger sounds
- Synchronized audio playback

### Component System

**Sound Component Lifecycle**:
1. **Init**: Load required sounds, set initial parameters
2. **Update**: Update 3D position, check distance culling
3. **Suspend**: Pause sounds when object is suspended
4. **Resume**: Resume sounds when object is active again
5. **Cleanup**: Stop sounds, free resources

## Performance Considerations

### Optimization Techniques

1. **Distance Culling**:
   - Don't play sounds beyond audible range
   - Stop sounds that move out of range
   - Use hysteresis to prevent popping

2. **Priority System**:
   - High-priority sounds (player actions) always play
   - Low-priority sounds (distant ambient) can be culled
   - Voice stealing for lower priority sounds

3. **Voice Limits**:
   - Maximum active sounds (128 positional)
   - Stop oldest/quietest sounds when limit reached
   - Reserve voices for critical sounds

4. **Streaming**:
   - Stream music and long sounds from disk
   - Small sounds kept in memory
   - Pre-buffer to prevent gaps

5. **Hardware Acceleration**:
   - Use hardware mixing when available
   - Hardware 3D audio processing
   - Fallback to software mixing

### Memory Management

**Memory Pools**:
- Pre-allocated sound buffers
- Wave table management
- Streaming buffer management
- Platform-specific memory alignment

**Loading Strategy**:
- Level-specific sound banks
- Permanent sounds always loaded
- Temporary sounds loaded on demand
- Streaming for music and large effects

## Debugging and Tools

### Debug Features

**Recommendations for Implementation**:
- Visual display of active sounds
- Volume level visualization
- 3D sound position markers
- Distance attenuation curves
- Voice count display
- Memory usage tracking

### Testing Considerations

- Test on various hardware (2.0, 5.1, 7.1 audio)
- Verify distance attenuation
- Check for audio popping
- Test voice stealing behavior
- Verify looping sound smoothness
- Check for memory leaks

## Key Files

### Gel/SoundFX
- `soundfx.h/cpp` - Core sound effects system
- `win32/p_sfx.h/cpp` - Win32 implementation (currently stubs)

### Gel/Music
- Platform-specific music streaming
- Playlist management

### Gel/Components
- `soundcomponent.h/cpp` - Generic sound component
- `VehicleSoundComponent.h/cpp` - Vehicle sounds

### Sk/Components
- `SkaterSoundComponent.h/cpp` - Skater sound effects
- `SkaterLoopingSoundComponent.h/cpp` - Continuous skater sounds

### Sk/Engine
- `sounds.h/cpp` - Physics-based sound triggers

## Usage Examples

### Playing a Simple Sound Effect

```cpp
// Play a one-shot sound effect
uint32 soundId = PlaySound(CRCD("Land_Concrete"), 
                          1.0f,   // volume
                          1.0f,   // pitch
                          100);   // priority
```

### Playing a Positional Sound

```cpp
// Play a 3D positioned sound
Mth::Vector soundPos = GetObjectPosition();
uint32 soundId = PlayPositionalSound(
    CRCD("Rail_Grind_Metal"),
    soundPos,
    0.8f,                          // volume
    1.0f,                          // pitch
    FEET_TO_INCHES(100.0f)        // dropoff distance
);
```

### Looping Sound with Updates

```cpp
// Start looping sound
uint32 loopId = PlayLoopingSound(CRCD("Skate_Roll"), 0.5f, 1.0f);

// Update each frame based on speed
void Update(float speed)
{
    float volume = CalculateRollVolume(speed);
    float pitch = CalculatePitchFromSpeed(speed);
    
    UpdateLoopingSound(loopId, volume, pitch);
}

// Stop when done
StopLoopingSound(loopId);
```

## Future Enhancements

### Potential Improvements

1. **Modern Audio APIs**:
   - Replace DirectSound with XAudio2 or OpenAL
   - Support for modern spatial audio (Windows Sonic, Dolby Atmos)
   - HRTF (Head-Related Transfer Function) for headphones

2. **Advanced Audio Features**:
   - Reverb zones
   - Audio obstruction/occlusion
   - Dynamic range compression
   - Real-time audio effects (EQ, filters)

3. **Asset Pipeline**:
   - Modern audio compression (Opus, Vorbis)
   - Audio middleware integration (FMOD, Wwise)
   - Automated audio asset management

4. **Debugging Tools**:
   - Visual audio mixer
   - Real-time waveform display
   - 3D audio visualization
   - Performance profiling

5. **Music System**:
   - Dynamic music system (adaptive soundtracks)
   - Smooth transitions based on gameplay
   - Multiple music layers

## Related Documentation

- [ARCHITECTURE.md](../ARCHITECTURE.md) - Overall system architecture
- [GAME_LOOP.md](../GAME_LOOP.md) - Audio update timing
- [STUB_FUNCTIONS.md](../platforms/STUB_FUNCTIONS.md) - Win32 audio stubs that need implementation
- [PHYSICS.md](PHYSICS.md) - Physics-driven audio events

## Implementation Status (Win32 Port)

### Current State

✅ **Audio Backend Architecture Implemented**

The PC port now has a working audio backend selection system:

**Completed**:
- ✅ Audio backend dispatcher in Win32 platform layer (`Code/Gel/Music/Win32/p_music.cpp`)
- ✅ Backend selection via CMake configuration (`-DAUDIO_BACKEND=<SDL2|OpenAL|FMOD|None>`)
- ✅ Preprocessor-based backend routing using `USE_SDL2_AUDIO`, `USE_OPENAL_AUDIO`, `USE_FMOD_AUDIO` defines
- ✅ Stub implementations for all three backends (SDL2, OpenAL, FMOD)
- ✅ All PCMAudio functions dispatch to selected backend:
  - `PCMAudio_Init()` / `PCMAudio_Deinit()`
  - `PCMAudio_Update()`
  - `PCMAudio_LoadMusicHeader()` / `PCMAudio_TrackExists()`
  - `PCMAudio_StartStreaming()` / `PCMAudio_StopStreaming()` / `PCMAudio_PauseStream()`
  - `PCMAudio_SetVolume()` / `PCMAudio_GetVolume()`
  - `PCMAudio_SetMusicVolume()` / `PCMAudio_GetMusicVolume()`

⚠️ **Backend Implementations Are Stubs**

While the dispatcher is complete, the actual backend implementations are stubs that need library integration:

**Needs Implementation for SDL2 Backend** (`Code/Gel/Music/SDL2/p_audio.cpp`):
- SDL audio initialization: `SDL_Init(SDL_INIT_AUDIO)`, `Mix_OpenAudio()`
- Music loading: `Mix_LoadMUS()`
- Playback control: `Mix_PlayMusic()`, `Mix_HaltMusic()`, `Mix_PauseMusic()`
- Volume control: `Mix_VolumeMusic()`
- Cleanup: `Mix_CloseAudio()`

**Needs Implementation for OpenAL Backend** (`Code/Gel/Music/OpenAL/p_audio.cpp`):
- OpenAL initialization: `alcOpenDevice()`, `alcCreateContext()`
- Buffer and source management: `alGenBuffers()`, `alGenSources()`
- Streaming audio: Buffer queuing and unqueuing
- 3D positioning: `alSourcefv()` for position
- Cleanup: `alcDestroyContext()`, `alcCloseDevice()`

**Needs Implementation for FMOD Backend** (`Code/Gel/Music/FMOD/p_audio.cpp`):
- FMOD system creation: `FMOD::System_Create()`, `system->init()`
- Stream creation: `system->createStream()`
- Playback: `system->playSound()`
- Update: `system->update()` (required each frame)
- Cleanup: `system->close()`, `system->release()`

### Implementation Priority

**Phase 1: Backend Dispatcher** ✅ **COMPLETE**
1. ✅ Audio backend selection system
2. ✅ Win32 platform dispatcher
3. ✅ CMake integration

**Phase 2: SDL2 Backend Implementation** (Recommended for initial implementation)
1. Initialize SDL2_mixer library
2. Implement basic music playback
3. Implement volume controls
4. Test with audio files

**Phase 3: OpenAL Backend** (For 3D audio support)
1. Initialize OpenAL device and context
2. Implement streaming audio
3. Add 3D positioning
4. Test spatial audio

**Phase 4: FMOD Backend** (Optional, for professional features)
1. Integrate FMOD SDK
2. Implement advanced audio features
3. Add DSP effects
4. Commercial license required for release

**High Priority** (Critical for basic functionality):
1. Choose and implement one backend (SDL2 recommended)
2. Basic music playback
3. Volume control

**Medium Priority**:
1. Sound effect playback
2. 3D positioning (if using OpenAL)
3. File format support (OGG, MP3, WAV)

**Low Priority**:
1. Advanced effects
2. Multiple backend support
3. Performance optimization

## Notes

The audio system was designed for the original console platforms (PS2, Xbox, GameCube) and has been updated for the PC port with a modern audio backend architecture.

### PC Port Audio System

**Architecture**: 
- The PC port uses a dispatcher pattern with selectable backends (SDL2, OpenAL, FMOD)
- Backend selection happens at compile-time via CMake configuration
- The Win32 platform layer routes calls to the appropriate backend

**Current Status**:
- ✅ Backend dispatcher implemented and functional
- ⚠️ Backend implementations are stubs awaiting library integration
- 📦 Libraries need to be installed separately (SDL2_mixer, OpenAL-soft, or FMOD SDK)

**For a modern port**:
- Start with SDL2 backend for simplicity (recommended)
- Use OpenAL backend for 3D spatial audio
- Consider FMOD for professional features (requires license)
- Add comprehensive debugging and profiling tools
- Implement modern audio formats (OGG, MP3, FLAC)
- Support modern audio formats and compression
- Ensure cross-platform compatibility

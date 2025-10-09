# Audio System

## Overview

The THUG audio system handles music playback, sound effects, and positional audio. The system is implemented in `Code/Gel/SoundFX/` and `Code/Gel/Music/` with platform-specific implementations for different target platforms.

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

### Win32/DirectSound Implementation

Located in `Code/Gel/SoundFX/win32/p_sfx.h/cpp`:

**Platform-Specific Features**:
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

⚠️ **Win32 audio implementation is mostly stubs** (see [STUB_FUNCTIONS.md](../platforms/STUB_FUNCTIONS.md))

**Needs Implementation**:
- DirectSound initialization
- 3D sound buffer creation
- Positional audio updates
- Sound streaming
- Music playback

### Implementation Priority

**High Priority** (Critical for basic functionality):
1. Audio device initialization
2. Simple sound playback
3. Volume control
4. Basic 3D positioning

**Medium Priority**:
1. Full 3D audio with doppler
2. Music streaming
3. Advanced effects

**Low Priority**:
1. Hardware acceleration
2. Advanced spatial audio
3. Effect processing

## Notes

The audio system was designed for the original console platforms (PS2, Xbox, GameCube) and requires significant work to fully support modern platforms. The Win32 implementation is largely incomplete and serves as stubs showing the intended interface.

For a modern port:
- Consider using established audio middleware (FMOD, Wwise)
- Implement modern spatial audio techniques
- Add comprehensive debugging and profiling tools
- Support modern audio formats and compression
- Ensure cross-platform compatibility

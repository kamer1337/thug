# Audio System Documentation

## Overview

The THUG audio system has been modernized with a Hardware Abstraction Layer (HAL) that supports multiple audio backends. This allows the game to work on different platforms without changing the core audio code.

## Architecture

### Hardware Abstraction Layer (HAL)

Located in `Code/Core/HAL/`, the HAL provides a platform-independent audio interface:

- **Audio.h** - Audio backend interface definitions
- **Audio.cpp** - Audio device factory and management
- **AudioOpenAL.h/cpp** - OpenAL backend implementation
- **AudioSDLMixer.h/cpp** - SDL2_mixer backend implementation (future)

### Audio Device Interface

The `AudioDevice` interface provides methods for:

**Device Management:**
- `Initialize()` - Initialize the audio device
- `Shutdown()` - Clean up and release resources
- `Update()` - Per-frame update (check voice states, etc.)

**Volume Control:**
- `SetMasterVolume(float volume)` - Set global audio volume (0-100%)
- `GetMasterVolume()` - Get current master volume

**Sound Effects:**
- `LoadSound(filename, looping, permanent)` - Load a sound file
- `UnloadSound(sound)` - Free a loaded sound
- `PlaySound(sound, volumeL, volumeR, pitch)` - Play a sound effect
- `StopSound(voice)` - Stop a playing sound
- `StopAllSounds()` - Stop all sound effects
- `IsSoundPlaying(voice)` - Check if a voice is active
- `SetSoundParams(voice, volumeL, volumeR, pitch)` - Update sound parameters
- `PauseSounds(pause)` - Pause/unpause all sounds

**Music Streaming:**
- `LoadMusicHeader(filename)` - Load music track list
- `PlayMusicTrack(filename)` - Play a music track
- `StopMusic()` - Stop music playback
- `PauseMusic(pause)` - Pause/unpause music
- `SetMusicVolume(volume)` - Set music volume
- `GetMusicStatus()` - Check music playback status

**Stream Playback:**
- `LoadStreamHeader(filename)` - Load stream definitions
- `PlayStream(checksum, channel, volumeL, volumeR, pitch)` - Play streaming audio
- `StopStream(channel)` - Stop a stream
- `SetStreamVolume(channel, volumeL, volumeR)` - Update stream volume
- `SetStreamPitch(channel, pitch)` - Update stream pitch
- `GetStreamStatus(channel)` - Check stream status

**3D Audio:**
- `SetListenerPosition(x, y, z)` - Set camera/listener position
- `SetListenerOrientation(atX, atY, atZ, upX, upY, upZ)` - Set listener orientation
- `SetSoundPosition(voice, x, y, z)` - Set 3D position for a sound

## Audio Backends

### NULL Backend (Default)

A stub implementation that provides no audio output. Used when:
- No audio libraries are available
- Audio is disabled in configuration
- Running in headless mode

### OpenAL Backend

Cross-platform 3D audio using OpenAL.

**Features:**
- 32 simultaneous voices for sound effects
- 4 simultaneous streams
- 3D positional audio
- Volume and pitch control
- Music playback support

**Building with OpenAL:**
```bash
cmake -DUSE_OPENAL=ON ..
```

**Requirements:**
- OpenAL library (libopenal)
- On Linux: `sudo apt-get install libopenal-dev`
- On macOS: OpenAL is built-in
- On Windows: Install OpenAL SDK

### SDL2_mixer Backend (Future)

Simple audio using SDL2_mixer.

**Features:**
- Easy to use
- Cross-platform
- Good for basic audio needs
- Music and sound effect playback

**Building with SDL2_mixer:**
```bash
cmake -DUSE_SDL_MIXER=ON ..
```

**Requirements:**
- SDL2 and SDL2_mixer libraries

## Platform Integration

### Win32 Platform

The Win32 platform implementation (`Code/Gel/SoundFX/Win32/` and `Code/Gel/Music/Win32/`) has been updated to use the HAL:

**Sound Effects (`p_sfx.cpp`):**
- `InitSoundFX()` - Initialize using HAL device
- `LoadSoundPlease()` - Load sounds through HAL
- `PlaySoundPlease()` - Play sounds through HAL
- `StopSoundPlease()` - Stop sounds through HAL
- `SetVoiceParameters()` - Update sound parameters

**Music/Streaming (`p_music.cpp`):**
- `PCMAudio_Init()` - Initialize streaming using HAL
- `PCMAudio_PlayMusicTrack()` - Play music through HAL
- `PCMAudio_PlayStream()` - Play streams through HAL
- `PCMAudio_SetMusicVolume()` - Control music volume

## Usage Example

```cpp
#include <core/HAL/Audio.h>

// Initialize audio at application startup
HAL::AudioDevice* device = HAL::CreateAudioDevice(HAL::AUDIO_BACKEND_OPENAL);
if (device && device->Initialize()) {
    device->SetMasterVolume(100.0f);
    
    // Load a sound
    HAL::AudioSound* sound = device->LoadSound("sounds/jump.wav", false, false);
    
    // Play the sound
    int voice = device->PlaySound(sound, 100.0f, 100.0f, 100.0f);
    
    // Update per frame
    device->Update();
    
    // Clean up
    device->UnloadSound(sound);
    device->Shutdown();
}
HAL::DestroyAudioDevice(device);
```

## Audio File Formats

The audio system is designed to support multiple formats:

**Sound Effects:**
- WAV (uncompressed)
- OGG Vorbis (compressed)

**Music/Streaming:**
- OGG Vorbis
- MP3 (if supported by backend)

## Implementation Status

### Completed
- ✅ HAL audio interface design
- ✅ NULL backend (stub implementation)
- ✅ OpenAL backend (basic implementation)
- ✅ Win32 platform integration
- ✅ Sound effect playback framework
- ✅ Music playback framework
- ✅ 3D positional audio support
- ✅ CMake build configuration

### In Progress
- ⚠️ OpenAL file loading (WAV/OGG)
- ⚠️ Stream playback implementation
- ⚠️ Music preloading support

### Planned
- 🔲 SDL2_mixer backend
- 🔲 FMOD backend (optional)
- 🔲 Audio effects (reverb, etc.)
- 🔲 Audio compression support
- 🔲 Dynamic voice allocation
- 🔲 Audio resource management

## Known Limitations

1. **File Loading**: Currently, sound loading is stubbed out. Full implementation requires:
   - WAV file parser
   - OGG Vorbis decoder
   - Audio format conversion

2. **Streaming**: Music and stream playback need:
   - Buffer management
   - Async file loading
   - Decode/playback threading

3. **3D Audio**: Basic 3D positioning is implemented, but advanced features need:
   - Doppler effect
   - Distance attenuation
   - Environmental reverb

4. **Platform Testing**: OpenAL backend tested on Linux only. Need to verify:
   - Windows functionality
   - macOS functionality
   - Performance optimization

## Future Enhancements

1. **Audio Compression**
   - Support for compressed formats (OGG, MP3)
   - Runtime decompression
   - Memory optimization

2. **Advanced 3D Audio**
   - HRTF (Head-Related Transfer Function)
   - Occlusion and obstruction
   - Environmental audio effects

3. **Audio Mixing**
   - Dynamic voice priority
   - Automatic ducking
   - Crossfading

4. **Performance**
   - Multi-threaded audio processing
   - Lock-free audio buffers
   - Optimized mixing

## References

- OpenAL Programming Guide: https://www.openal.org/documentation/
- SDL2_mixer Documentation: https://www.libsdl.org/projects/SDL_mixer/
- Original DirectSound documentation (for reference)

## Related Files

- `Code/Core/HAL/Audio.h` - Audio interface
- `Code/Core/HAL/Audio.cpp` - Audio implementation
- `Code/Core/HAL/AudioOpenAL.h/cpp` - OpenAL backend
- `Code/Gel/SoundFX/Win32/p_sfx.h/cpp` - Win32 sound FX
- `Code/Gel/Music/Win32/p_music.h/cpp` - Win32 music
- `CMakeLists.txt` - Build configuration
- `TASKS.md` - Related tasks
- `docs/platforms/WIN32.md` - Platform documentation
- `docs/platforms/STUB_FUNCTIONS.md` - Stub function reference
